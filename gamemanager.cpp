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
    m_map(0), m_view(view), m_roundsPlayed(0)
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
    m_timer.setInterval(250); // 10 times a second
    m_timer.setSingleShot(false);

    m_server.listen(QHostAddress::Any, 54321);

    connect(&m_timer, SIGNAL(timeout()), SLOT(gameTick()));
    connect(&m_timer, SIGNAL(timeout()), SIGNAL(tick()));
    connect(&m_server, SIGNAL(newConnection()), SLOT(clientConnect()));
    connect(this, SIGNAL(gameOver()), SLOT(endRound()));

    // Watch filesystem for map changes
    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    watcher->addPath("maps/");
    connect(watcher, SIGNAL(directoryChanged(QString)), SIGNAL(mapsChanged()));

    // Set up sound effects
    m_explosion.setSource(QUrl::fromLocalFile("sound/explosion.wav"));
    m_explosion.setVolume(0.25f);
    m_backgroundLoop.setSource(QUrl::fromLocalFile("sound/drumloop2.wav"));
    m_backgroundLoop.setVolume(0.25f);
    m_backgroundLoop.setLoopCount(QSoundEffect::Infinite);
    m_death.setSource(QUrl::fromLocalFile("sound/death.wav"));
    m_death.setVolume(0.25f);

    QSettings setting("sound");
    setSoundEnabled(setting.value("enabled", false).toBool());
}

GameManager::~GameManager()
{
    // Ensure we don't crash because we delete stuff before it disconnects
    for (int i=0; i<playerCount(); i++) {
        if (player(i)->networkClient()) {
            disconnect(player(i)->networkClient(), SIGNAL(clientDisconnected()), this, SLOT(clientDisconnected()));
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
        m_explosion.stop();
        m_death.stop();
    }
    m_soundEnabled = enabled;

    emit soundEnabledChanged();
}

QStringList GameManager::maps()
{
    QStringList ret;

    ret << ":/maps/default.map"
        << ":/maps/Arena.map";

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
    for (int i=playerCount()-1; i>=0; --i) {
        if (playerCount() <= m_map->startingPositions().count()) {
            break;
        }
        if (!player(i)->networkClient()) continue; // Don't kick out human player

        m_players.takeAt(i)->deleteLater();
    }

    // Update positions and id
    for (int i=0; i<playerCount(); i++) {
        player(i)->setPosition(m_map->startingPositions()[i]);
        player(i)->setId(i);
    }

    exportPlayerList();
    m_view->rootContext()->setContextProperty("map", map);

    connect(m_map, SIGNAL(explosionAt(QPoint)), SLOT(explosionAt(QPoint)));
}

void GameManager::playBombSound()
{
    if (m_soundEnabled) {
        m_explosion.play();
    }
}

void GameManager::explosionAt(const QPoint &position)
{
    for (int i=0; i<playerCount(); i++) {
        if (player(i)->position() == position) {
            if (m_soundEnabled) {
                m_death.play();
            }
            player(i)->takeLife();
        }
    }
}

void GameManager::endRound()
{
    m_timer.stop();

    for (int i=0; i<playerCount(); i++) {
        if (!player(i)->networkClient()) continue;
        player(i)->networkClient()->sendEndOfRound();
    }

    if (m_roundsPlayed < 5) {
        for (int i=0; i<playerCount(); i++) {
            if (player(i)->isAlive()) {
                player(i)->addWin();
                break;
            }
        }
        m_roundsPlayed++;
        QTimer::singleShot(1000, this, SLOT(startRound()));
    } else {
        QQuickItem *endScreen = qobject_cast<QQuickItem*>(m_view->rootObject()->findChild<QObject*>("endScreen"));
        if (endScreen) {
            endScreen->setProperty("opacity", 1.0);
        }
        m_roundsPlayed = 0;
    }
}

void GameManager::startRound()
{
    if (m_players.isEmpty()) {
        return;
    }

    for (int i=0; i<playerCount(); i++) {
        player(i)->resetLives();
        player(i)->setPosition(m_map->startingPositions()[i]);
    }
    loadMap(m_map->name());

    // Do not allow to change name after game has started
    for (int i=0; i<playerCount(); i++) {
        player(i)->networkClient()->disconnect(SIGNAL(nameChanged(QString)));
    }
    m_timer.start();
}

void GameManager::gameTick()
{
    for (int i=0; i<playerCount(); i++) {
        QString command = player(i)->command();
        if (command.isEmpty()) {
            continue;
        }

        QPoint position = player(i)->position();

        if (command == "UP") {
            position.setY(position.y() - 1);
        } else if (command == "DOWN") {
            position.setY(position.y() + 1);
        } else if (command == "LEFT") {
            position.setX(position.x() - 1);
        } else if (command == "RIGHT") {
            position.setX(position.x() + 1);
        } else if (command == "BOMB") {
            m_map->addBomb(position, player(i));
            continue;
        } else {
            continue;
        }

        bool canWalk = m_map->isValidPosition(position);

        if (canWalk) {
            for (int j=0; j<playerCount(); j++) {
                if (player(j)->position() == position) {
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
            player(i)->setPosition(position);
        }
    }

    int dead = 0;
    for(int i=0; i<playerCount(); i++) {
        if (!player(i)->isAlive()) {
            dead++;
        }
    }

    if (dead > 0 && m_players.size() - dead < 2) {
        emit gameOver();
    } else {
        QList<Player*> players;
        for (int i=0; i<playerCount(); i++) {
            if (player(i)->isAlive())
                players.append(player(i));
        }
        for (int i=0; i<players.length(); i++) {
            if (!player(i)->networkClient()) continue;

            QList<Player*> list = players;
            list.takeAt(i);
            player(i)->networkClient()->sendState(list, m_map, players[i]);
        }
    }
}

void GameManager::clientConnect()
{
    QTcpSocket *socket = m_server.nextPendingConnection();

    if (playerCount() >= m_map->startingPositions().count()) {
        socket->disconnect();
        socket->deleteLater();
        return;
    }

    addPlayer(new NetworkClient(socket));
}

void GameManager::clientDisconnected()
{
    if (m_timer.isActive()) {
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
    for (int i=0; i<playerCount(); i++) {
        player(i)->setId(i);
    }
    exportPlayerList();
}

Player *GameManager::player(int id)
{
    if (id > m_players.size()) return NULL;
    return qobject_cast<Player*>(m_players[id]);
}

int GameManager::playerCount()
{
    return m_players.size();
}

void GameManager::addPlayer(NetworkClient *client)
{
    if (playerCount() >= m_map->startingPositions().count()) {
        return;
    }

    Player *player = new Player(this, playerCount(), client);
    player->setPosition(m_map->startingPositions().at(player->id()));
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

void GameManager::removeHumanPlayers()
{
    for (int i=0; i<playerCount(); i++) {
        if (!player(i)->networkClient()) {
            m_players.takeAt(i)->deleteLater();
            break;
        }
    }
    for (int i=0; i<playerCount(); i++) {
        player(i)->setId(i);
    }
    exportPlayerList();
}

QString GameManager::address()
{
    return m_server.serverAddress().toString() + QLatin1Char(':') + QString::number(m_server.serverPort());
}

void GameManager::togglePause()
{
    if (m_timer.isActive()) {
        m_timer.stop();
    } else {
        m_timer.start();
    }
}

void GameManager::stopGame()
{
    endRound();
    m_roundsPlayed = 0;
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
