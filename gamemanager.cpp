#include "gamemanager.h"

#include "player.h"
#include "networkclient.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QPoint>
#include <QList>
#include <QQmlComponent>
#include <QTcpSocket>
#include <QSettings>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>

#include <qmath.h> // Fuck windows aight

#include <algorithm>

GameManager::GameManager() : QObject(),
    m_roundsPlayed(0),
    m_gameRunning(false),
    m_maxRounds(MAX_ROUNDS),
    m_randomGenerator(m_randomDevice()),
    m_map(new Map(this))
{
    m_map->loadMap(":/map1.map");

    // Set up gametick timer
    m_tickTimer.setInterval(DEFAULT_TICKINTERVAL);
    m_tickTimer.setSingleShot(false);
    m_elapsedTimer.start();

    // Set up timer for delayed starting of rounds
    m_startTimer.setInterval(750);
    m_startTimer.setSingleShot(true);

    m_server.listen(QHostAddress::Any, 54321);

    connect(&m_startTimer, &QTimer::timeout, this, &GameManager::startRound);
    connect(&m_tickTimer, &QTimer::timeout, this, &GameManager::gameTick);
    connect(&m_server, &QTcpServer::newConnection, this, &GameManager::clientConnect);
}

GameManager::~GameManager()
{
    // Ensure we don't crash because we delete stuff before it disconnects
    for (int i=0; i<m_players.count(); i++) {
        if (m_players[i]->networkClient()) {
            disconnect(m_players[i]->networkClient(), &NetworkClient::clientDisconnected, this, &GameManager::clientDisconnected);
        }
    }
}

QList<QObject*> GameManager::players() const
{
    QList<Player*> playerList = m_players;

    // Sort players by wins, and then scores/points
    std::sort(playerList.begin(), playerList.end(), Player::comparePlayers);

    QList<QObject*> objectList;
    for (Player *playerObject : playerList) {
        objectList.append(playerObject);
    }

    return objectList;
}

QString GameManager::version()
{
    QString versionString;

#ifdef APP_VERSION
    versionString += QStringLiteral(APP_VERSION);
#endif

    versionString += " // build time: " + QLatin1String(__TIME__) + ' ' + QLatin1String(__DATE__);
    return versionString;
}

void GameManager::endRound()
{
    m_tickTimer.stop();

    emit roundOver();

    for (int i=0; i<m_players.count(); i++) {
        if (!m_players[i]->networkClient()) continue;
        m_players[i]->networkClient()->sendEndOfRound();
    }

    for (int i=0; i<m_players.count(); i++) {
        if (m_players[i]->isAlive()) {
            m_players[i]->addWin();
        }
    }

    m_roundsPlayed++;
    emit roundsPlayedChanged();

    if (m_roundsPlayed < m_maxRounds) {
        // Show the countdown again
        if (m_startTimer.interval() > 0) {
            emit showCountdown();
        }
        m_startTimer.start();
    } else {
        QFile scoreFile("scores.txt");
        if (!scoreFile.open(QIODevice::WriteOnly)) {
            return;
        }

        QList<Player*> playerList = m_players;
        std::sort(playerList.begin(), playerList.end(), Player::comparePlayers);

        qDebug() << "Result:";
        for(Player *player : playerList) {
            qDebug() << player->name()
                     << "Wins:" << player->wins()
                     << "Score:" << player->score();
            scoreFile.write(player->name().toUtf8() + ' ' +
                            QByteArray::number(player->wins()) + ' ' +
                            QByteArray::number(player->score()));
        }
    }
}

void GameManager::startRound()
{
    if (m_players.isEmpty()) {
        return;
    }

    if (!m_gameRunning) {
        return;
    }

    resetPositions();

    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setCommand(QString());
        m_players[i]->setAlive(true);
    }

    // Do not allow to change name after game has started
    for (int i=0; i<m_players.count(); i++) {
        if (!m_players[i]->networkClient()) {
            continue;
        }

        m_players[i]->networkClient()->disconnect(m_players[i]->networkClient(), &NetworkClient::nameChanged, m_players[i], &Player::setName);
    }

    m_tickTimer.start();
}

void GameManager::startGame()
{
    if (m_gameRunning) {
        return;
    }

    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->resetScore();
    }


    m_roundsPlayed = 0;
    emit roundsPlayedChanged();

    m_gameRunning = true;
    emit gameRunningChanged();

    if (m_startTimer.interval() > 0) {
        emit showCountdown();
    }

    m_elapsedTimer.restart();
    m_startTimer.start();
}

void GameManager::gameTick()
{
    // Randomize the order we process players in
    QList<Player*> players = m_players;
    std::shuffle(players.begin(), players.end(), m_randomGenerator);

    int dead = 0;
    for(Player *player : players) {
        if (!player->isAlive()) {
            dead++;
            continue;
        }

        // Get last command from player, and if there is one do the appropriate thing
        // This also resets the command to "", so we don't do a command more than once
        QString command = player->command();
        if (command.isEmpty()) {
            continue;
        }

        int newX = player->x();
        int newY = player->y();
        if (command == "up") {
            newY--;
        } else if (command == "down") {
            newY++;
        } else if (command == "left") {
            newX--;
        } else if (command == "right") {
            newX++;
        }
        if (newX < 0) {
            newX = m_map->width() - 1;
        }
        if (newX >= m_map->width()) {
            newX = 0;
        }

        if (m_map->isValidPosition(newX, newY)) {
            Map::Powerup powerup = m_map->takePowerup(newX, newY);
            if (powerup == Map::NormalPellet) {
                player->addScore(1);
            } else if (powerup == Map::SuperPellet) {
                player->addScore(10);
            }
            player->setPosition(newX, newY);
        }
    }

    // Check if there's only one player left, but only if there is more than one player playing
    if (dead > 0 && players.size() - dead < 2) {
        endRound();
        return;
    }

    // Send status updates to all connected players
    for(Player *player : players) {
        if (!player->networkClient()) {
            continue;
        }

        player->networkClient()->sendState(serializeForPlayer(player));
    }

    emit secondsElapsedChanged();
}

void GameManager::clientConnect()
{
    QTcpSocket *socket = m_server.nextPendingConnection();

    if (!socket) {
        qWarning() << "We expected a socket here, but got none";
        return;
    }

    if (m_players.count() >= MAX_PLAYERS || m_tickTimer.isActive() || m_gameRunning) {
        socket->disconnect();
        socket->deleteLater();
        return;
    }

    addPlayer(new NetworkClient(socket));
}

void GameManager::clientDisconnected()
{
    if (m_tickTimer.isActive()) {
        return;
    }

    // FIXME fix this shit
    Player *playerObject = qobject_cast<Player*>(sender());
    if (!playerObject) {
        qWarning() << "GameManager: invalid sender for disconnect signal";
        return;
    }
    int index = m_players.indexOf(playerObject);
    if (index < 0) {
        qWarning() << "GameManager: unable to find disconnecting client.";
        return;
    }

    m_players.takeAt(index)->deleteLater();
    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setId(i);
    }

    emit playersChanged();
}

void GameManager::addPlayer(NetworkClient *client)
{
    if (m_players.count() >= MAX_PLAYERS) {
        client->deleteLater();
        return;
    }

    Player *player = new Player(this, m_players.count(), client);

    m_players.append(player);

    resetPositions();

    if (!client) {
        connect(this, &GameManager::humanMove, player, &Player::setCommand);
        player->setName("Local user");
    } else {
        player->setName(client->remoteName());
        connect(player, &Player::clientDisconnected, this, &GameManager::clientDisconnected);
    }

    emit playersChanged();
}
void GameManager::kick(int index)
{
    if (index < 0 || index > m_players.count()) {
        qWarning() << "Asked to kick invalid index" << index;
        return;
    }

    if (m_players.at(index)->networkClient()) {
        m_players.at(index)->networkClient()->kick();
    } else {
        m_players.takeAt(index)->deleteLater();
    }

    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setId(i);
    }

    emit playersChanged();
}

QString GameManager::timeElapsed()
{
    const qint64 seconds = m_elapsedTimer.elapsed();
    return QString::number(qFloor(seconds / 60000)).rightJustified(2, '0') + ':' + QString::number(seconds / 1000 % 60).rightJustified(2, '0');
}

void GameManager::removeHumanPlayer()
{
    for (int i=0; i<m_players.count(); i++) {
        if (!m_players[i]->networkClient()) {
            kick(i);
            return;
        }
    }
}

void GameManager::setTickInterval(int interval)
{
    m_tickTimer.setInterval(interval);
}

void GameManager::togglePause()
{
    if (m_tickTimer.isActive()) {
        m_tickTimer.stop();
    } else {
        m_tickTimer.start();
    }
}

void GameManager::stopGame()
{
    m_startTimer.stop(); // Just in case
    m_tickTimer.stop();
    m_gameRunning = false;
    emit gameRunningChanged();

    QMutableListIterator<Player*> it(m_players);
    while(it.hasNext()) {
        Player *player = it.next();
        if (player->isDisconnected()) {
            player->deleteLater();
            it.remove();
        }
    }

    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setId(i);
    }

    m_roundsPlayed = 0;
    emit roundsPlayedChanged();

    emit playersChanged();
}

void GameManager::resetPositions()
{
    if (m_players.count() == 0) {
        return;
    }
    QVector<QPoint> positions = m_map->startingPositions();
    if (m_players.count() > positions.count()) {
        qWarning() << "Too many players";
        return;
    }

    // Randomize the position processing order
    QList<Player*> players = m_players;
    std::shuffle(players.begin(), players.end(), m_randomGenerator);
    int playerCount = players.count();

    for (int i=0; i<playerCount; i++) {
        players[i]->setPosition(positions[i].x(), positions[i].y());
    }
}

QJsonObject GameManager::serializeForPlayer(Player *player)
{
    QJsonObject gamestateObject;
    gamestateObject["you"] = player->serialize();

    QJsonArray playersArray;
    for (Player *otherPlayer : m_players) {
        if (otherPlayer == player) {
            continue;
        }
        if (!otherPlayer->isAlive()) {
            continue;
        }

        playersArray.append(otherPlayer->serialize());
    }
    gamestateObject["others"] = playersArray;

    return gamestateObject;
}
