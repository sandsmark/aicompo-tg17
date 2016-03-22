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

#include <cmath>
#include <algorithm>

GameManager::GameManager() : QObject(),
    m_roundsPlayed(0),
    m_gameRunning(false),
    m_maxRounds(MAX_ROUNDS),
    m_randomGenerator(m_randomDevice())
{
    // Set up gametick timer
    m_tickTimer.setInterval(DEFAULT_TICKINTERVAL);
    m_tickTimer.setSingleShot(false);

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

    for (int i=0; i<m_missiles.count(); i++) {
        m_missiles[i]->deleteLater();
    }
    m_missiles.clear();

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
                     << "Score:" << player->score()
                     << "Energy:" << player->energy();
            scoreFile.write(player->name().toUtf8() + ' ' +
                            QByteArray::number(player->wins()) + ' ' +
                            QByteArray::number(player->score()) + ' ' +
                            QByteArray::number(player->energy()) + '\n');
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
        m_players[i]->setEnergy(START_ENERGY);
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


    for (int i=0; i<m_missiles.count(); i++) {
        m_missiles[i]->deleteLater();
    }
    m_missiles.clear();

    m_roundsPlayed = 0;
    emit roundsPlayedChanged();

    m_gameRunning = true;
    emit gameRunningChanged();

    if (m_startTimer.interval() > 0) {
        emit showCountdown();
    }

    m_startTimer.start();
}

void GameManager::gameTick()
{
    QMutableListIterator<Missile*> missileIterator(m_missiles);
    while (missileIterator.hasNext()) {
        Missile *missile = missileIterator.next();

        missile->doMove();

        // The missile doMove() checks if it has hit the sun, and if so sets alive to false
        if (!missile->isAlive()) {
            emit explosion(missile->position());
            missile->deleteLater();
            missileIterator.remove();
            continue;
        }

        // For seeking missiles, we need to find the closest player
        Player *closest = nullptr;
        qreal closestDX = 0, closestDY = 0;

        for(int i=0; i<m_players.count(); i++) {
            if (!m_players[i]->isAlive()) {
                continue;
            }

            // Missiles can't hit and seeking missiles won't target their own player
            if (missile->owner() == m_players[i]->id()) {
                continue;
            }

            // Check if missile has hit a player
            const qreal dx = m_players[i]->position().x() - missile->position().x();
            const qreal dy = m_players[i]->position().y() - missile->position().y();
            if (hypot(dx, dy) < 0.1) {
                m_players[i]->decreaseEnergy(MISSILE_DAMAGE);
                m_players[missile->owner()]->increaseEnergy(MISSILE_DAMAGE);
                m_players[missile->owner()]->addScore(1);
                emit explosion(missile->position());
                missile->deleteLater();
                missileIterator.remove();
                break;
            }

            // For seeking missiles
            if (!closest || hypot(dx, dy) < hypot(closestDX, closestDY)) {
                closest = m_players[i];
                closestDX = dx;
                closestDY = dy;
                continue;
            }
        }

        if (missile->type() == Missile::Seeking && closest) {
            missile->setRotation(atan2(closestDY, closestDX));
        }
    }


    // Randomize the order we process players in
    QList<Player*> players = m_players;
    std::shuffle(players.begin(), players.end(), m_randomGenerator);

    int dead = 0;
    for(Player *player : players) {
        if (!player->isAlive()) {
            dead++;
            continue;
        }

        player->doMove();
        player->decreaseEnergy(1);

        // Get last command from player, and if there is one do the appropriate thing
        // This also resets the command to "", so we don't do a command more than once
        QString command = player->command();
        if (command.isEmpty()) {
            continue;
        }

        if (command == "ACCELERATE") {
            player->accelerate();
        } else if (command == "LEFT") {
            player->rotate(-ROTATE_AMOUNT);
        } else if (command == "RIGHT") {
            player->rotate(ROTATE_AMOUNT);
        } else if (command == "MISSILE") {
            player->decreaseEnergy(MISSILE_COST);
            Missile *missile = new Missile(Missile::Normal, player->position(), player->rotation(), player->id(), this);
            m_missiles.append(missile);
            emit missileCreated(missile);
        } else if (command == "SEEKING") {
            player->decreaseEnergy(SEEKING_MISSILE_COST);
            Missile *missile = new Missile(Missile::Seeking, player->position(), player->rotation(), player->id(), this);
            m_missiles.append(missile);
            emit missileCreated(missile);
        } else if (command == "MINE") {
            player->decreaseEnergy(MINE_COST);
            Missile *missile = new Missile(Missile::Mine, player->position(), player->rotation(), player->id(), this);
            m_missiles.append(missile);
            emit missileCreated(missile);
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
}

void GameManager::clientConnect()
{
    QTcpSocket *socket = m_server.nextPendingConnection();

    if (!socket) {
        qWarning() << "We expected a socket here, but got none";
        return;
    }

    if (m_players.count() >= MAX_PLAYERS || m_tickTimer.isActive()) {
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

    // Randomize the position processing order
    QList<Player*> players = m_players;
    std::shuffle(players.begin(), players.end(), m_randomGenerator);
    int playerCount = players.count();

    // Rotate by a random amount
    std::uniform_real_distribution<qreal> randomDistribution(0, M_PI / 2.0);
    qreal angleOffset = randomDistribution(m_randomGenerator);

    for (int i=0; i<playerCount; i++) {
        qreal angle = i * M_PI * 2.0 / playerCount + angleOffset;
        players[i]->setPosition(QPointF(cos(angle) * 0.5, sin(angle) * 0.5));
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

    QJsonArray missilesArray;
    for (Missile *missile : m_missiles) {
        missilesArray.append(missile->serialize());
    }
    gamestateObject["missiles"] = missilesArray;

    return gamestateObject;
}
