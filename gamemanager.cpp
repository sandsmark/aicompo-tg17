#include "gamemanager.h"

#include "bomb.h"
#include "tile.h"
#include "map.h"
#include "player.h"
#include "networkclient.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QQmlContext>
#include <QQuickView>
#include <QQuickItem>
#include <QPoint>
#include <QList>
#include <QQmlComponent>
#include <QTcpSocket>
#include <QFileSystemWatcher>
#include <QSettings>

GameManager::GameManager(QQuickView *view) : QObject(view),
    m_map(0), m_view(view), m_roundsPlayed(0), m_ticksLeft(-1)
{
    loadMap(":/maps/default.map");

    if (!m_map) {
        qWarning() << "GameManager: Unable to load default map!";
        return;
    }

    // Add QML objects
    exportPlayerList();
    m_view->rootContext()->setContextProperty("game", QVariant::fromValue(this));

    // Set up gametick timer
    m_tickTimer.setInterval(250); // 10 times a second
    m_tickTimer.setSingleShot(false);

    m_server.listen(QHostAddress::Any, 54321);

    connect(&m_tickTimer, SIGNAL(timeout()), SLOT(gameTick()));
    connect(&m_tickTimer, SIGNAL(timeout()), SIGNAL(tick()));
    connect(&m_server, SIGNAL(newConnection()), SLOT(clientConnect()));
    connect(this, SIGNAL(gameOver()), SLOT(endRound()));

    // Watch filesystem for map changes
    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    watcher->addPath("maps/");
    connect(watcher, SIGNAL(directoryChanged(QString)), SIGNAL(mapsChanged()));

    // Set up sound effects
    for(int i=0; i<8; i++) {
        QSoundEffect *explosion = new QSoundEffect(this);
        explosion->setSource(QUrl::fromLocalFile("sound/explosion0" + QString::number(i+1) + ".wav"));
        explosion->setVolume(1.0f);
        m_explosions.append(explosion);
    }
    m_backgroundLoop.setSource(QUrl::fromLocalFile("sound/bgm.wav"));
    m_backgroundLoop.setVolume(1.0f);
    m_backgroundLoop.setLoopCount(QSoundEffect::Infinite);
    m_death.setSource(QUrl::fromLocalFile("sound/death.wav"));
    m_death.setVolume(1.0f);

    QSettings setting("sound");
    setSoundEnabled(setting.value("enabled", false).toBool());
}

GameManager::~GameManager()
{
    // Ensure we don't crash because we delete stuff before it disconnects
    for (int i=0; i<m_players.count(); i++) {
        if (m_players[i]->networkClient()) {
            disconnect(m_players[i]->networkClient(), SIGNAL(clientDisconnected()), this, SLOT(clientDisconnected()));
        }
    }
}

void GameManager::setSoundEnabled(bool enabled)
{
    if (enabled == m_soundEnabled) return;

    QSettings setting("sound");
    setting.setValue("enabled", enabled);

    if (enabled) {
        m_backgroundLoop.play();
    } else {
        m_backgroundLoop.stop();
        for(int i=0; i<8; i++) {
            m_explosions[i]->stop();
        }
        m_death.stop();
    }
    m_soundEnabled = enabled;

    emit soundEnabledChanged();
}

QStringList GameManager::maps()
{
    QStringList ret;

    ret << ":/maps/default.map"
        << ":/maps/Arena.map"
        << ":/maps/HugeArena.map";

    QDir mapDir("maps/");
    foreach (const QString file, mapDir.entryList(QDir::Files)) {
        ret << "maps/" + file;
    }

    return ret;
}

void GameManager::loadMap(const QString &path)
{
    Map *map = new Map(this, path);

    if (!map->isValid()) {
        qWarning() << "Map: Parsed map isn't expected size";
        delete map;
        return;
    }

    if (m_map)
        m_map->deleteLater();
    m_map = map;


    // Kick out players if too many
    for (int i=m_players.count()-1; i>=0; --i) {
        if (m_players.count() <= m_map->startingPositions().count()) {
            break;
        }
        if (!m_players[i]->networkClient()) continue; // Don't kick out human player

        m_players.takeAt(i)->deleteLater();
    }


    // Get a randomized list of unoccupied starting positions
    QList<QPoint> startPositions = m_map->startingPositions();
    for (int index = startPositions.count() - 1; index > 0; --index) {
        qSwap(startPositions[index], startPositions[qrand() % (index + 1)]);
    }

    // Update positions and id
    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setPosition(startPositions[i]);
        m_players[i]->setId(i);
    }

    exportPlayerList();
    m_view->rootContext()->setContextProperty("map", map);

    connect(m_map, SIGNAL(explosionAt(QPoint)), SLOT(explosionAt(QPoint)));
}

void GameManager::playBombSound()
{
    if (m_soundEnabled) {
        m_explosions[qrand() % 8]->play();
    }
}

void GameManager::explosionAt(const QPoint &position)
{
    for (int i=0; i<m_players.count(); i++) {
        if (m_players[i]->position() == position) {
            if (m_soundEnabled) {
                m_death.play();
            }
            m_players[i]->setAlive(false);
        }
    }
}

void GameManager::endRound()
{
    m_tickTimer.stop();

    for (int i=0; i<m_players.count(); i++) {
        if (!m_players[i]->networkClient()) continue;
        m_players[i]->networkClient()->sendEndOfRound();
    }

    if (m_roundsPlayed >= 5) {
        QQuickItem *endScreen = qobject_cast<QQuickItem*>(m_view->rootObject()->findChild<QObject*>("endScreen"));
        if (endScreen) {
            endScreen->setProperty("opacity", 1.0);
        }
        m_roundsPlayed = 0;
        emit roundsPlayedChanged();
        return;
    }

    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setCommand(QString());
        if (m_players[i]->isAlive()) {
            m_players[i]->addWin();
            break;
        }
    }
    m_roundsPlayed++;
    emit roundsPlayedChanged();
    QTimer::singleShot(1000, this, SLOT(startRound()));
}

void GameManager::startRound()
{
    if (m_players.isEmpty()) {
        return;
    }

    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setAlive(true);
    }
    loadMap(m_map->name());

    // Do not allow to change name after game has started
    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->networkClient()->disconnect(SIGNAL(nameChanged(QString)));
    }

    m_ticksLeft = m_map->maxTicks();
    m_tickTimer.start();
}

void GameManager::resetScores()
{
    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->resetScore();
    }
}

void GameManager::gameTick()
{
    m_ticksLeft--;
    if (m_ticksLeft == 0) {
        m_map->explodeEverything();
    } else if (m_ticksLeft < 0) {
        int factor = 160 / (m_ticksLeft ? m_ticksLeft : 1);
        if (m_ticksLeft % (factor ? factor : 1) == 0)
            m_map->addRandomBomb();
    }

    QList<QPointer<Player> > players = m_players;
    for (int index = players.count() - 1; index > 0; --index) {
        qSwap(players[index], players[qrand() % (index + 1)]);
    }

    for (int i=0; i<players.count(); i++) {
        QString command = players[i]->command();
        if (command.isEmpty()) {
            continue;
        }

        QPoint position = players[i]->position();

        if (command == "UP") {
            position.setY(position.y() - 1);
        } else if (command == "DOWN") {
            position.setY(position.y() + 1);
        } else if (command == "LEFT") {
            position.setX(position.x() - 1);
        } else if (command == "RIGHT") {
            position.setX(position.x() + 1);
        } else if (command == "BOMB") {
            m_map->addBomb(position, players[i]);
            continue;
        } else {
            continue;
        }

        bool canWalk = m_map->isValidPosition(position);

        if (canWalk) {
            for (int j=0; j<players.count(); j++) {
                if (players[j]->isAlive() && players[j]->position() == position) {
                    canWalk = false;
                }
            }
        }
        if (canWalk) {
            foreach(const Bomb *bomb, m_map->bombs()) {
                if (bomb->position() == position) {
                    canWalk = false;
                }
            }
        }

        if (canWalk) {
            players[i]->setPosition(position);
        }
    }

    int dead = 0;
    for(int i=0; i<players.count(); i++) {
        if (!players[i]->isAlive()) {
            dead++;
        }
    }

    if (dead > 0 && players.size() - dead < 2) {
        emit gameOver();
    } else {
        QList<Player*> players;
        for (int i=0; i<m_players.count(); i++) {
            if (m_players[i]->isAlive()) {
                players.append(m_players[i]);
            }
        }
        for (int i=0; i<players.length(); i++) {
            if (!players[i]->networkClient()) continue;

            QList<Player*> list = players;
            list.takeAt(i);
            players[i]->networkClient()->sendState(list, m_map, players[i]);
        }
    }
}

void GameManager::clientConnect()
{
    QTcpSocket *socket = m_server.nextPendingConnection();

    if (m_players.count() >= m_map->startingPositions().count() || m_tickTimer.isActive()) {
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
    exportPlayerList();
}

void GameManager::addPlayer(NetworkClient *client)
{
    if (m_players.count() >= m_map->startingPositions().count()) {
        return;
    }

    Player *player = new Player(this, m_players.count(), client);

    // Get a list of unoccupied starting positions
    QList<QPoint> freePositions = m_map->startingPositions();
    foreach(Player *player, m_players) {
        freePositions.removeAll(player->position());
    }

    // Add player to a random starting position
    player->setPosition(freePositions.at(qrand() % freePositions.size()));

    m_players.append(player);

    if (!client) {
        connect(m_view->rootObject(), SIGNAL(userMove(QString)), player, SLOT(setCommand(QString)));
        player->setName("Local user");
    } else {
        player->setName(client->remoteName());
        connect(player, SIGNAL(clientDisconnected()), SLOT(clientDisconnected()));
    }

    // We need to reset the property, the qobjectlist model doesn't automatically update
    exportPlayerList();
}
void GameManager::kick(int index)
{
    if (m_players.at(index)->networkClient()) {
        m_players.at(index)->networkClient()->kick();
    } else {
        m_players.takeAt(index)->deleteLater();
    }
    exportPlayerList();
}

void GameManager::removeHumanPlayers()
{
    for (int i=0; i<m_players.count(); i++) {
        if (!m_players[i]->networkClient()) {
            m_players.takeAt(i)->deleteLater();
            break;
        }
    }
    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setId(i);
    }
    exportPlayerList();
}

QString GameManager::address()
{
    return m_server.serverAddress().toString() + QLatin1Char(':') + QString::number(m_server.serverPort());
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
    m_tickTimer.stop();
    foreach(QPointer<Player> player, m_players) {
        if (player->isDisconnected()) {
            player->deleteLater();
        }
    }
    exportPlayerList();
}

void GameManager::exportPlayerList()
{
    QList<QObject*> playerList;
    foreach(Player *playerObject, m_players) {
        playerList.append(playerObject);
    }

    // We need to reset the property, the qobjectlist model doesn't automatically update
    m_view->rootContext()->setContextProperty("players", QVariant::fromValue(playerList));
}
