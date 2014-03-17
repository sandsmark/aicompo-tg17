#include "gamemanager.h"

#include "tile.h"
#include "map.h"
#include "player.h"
#include "networkclient.h"

#include <QDebug>
#include <QFile>
#include <QQmlContext>
#include <QQuickView>
#include <QQuickItem>
#include <QPoint>
#include <QList>
#include <QQmlComponent>
#include <QTcpSocket>

GameManager::GameManager(QQuickView *view) : QObject(view),
    m_map(0), m_view(view), m_roundsPlayed(0)
{
    loadMap("default");

    if (!m_map) {
        qWarning() << "GameManager: Unable to load default map!";
        return;
    }

    m_view->rootContext()->setContextProperty("players", QVariant::fromValue(m_players));
    m_view->rootContext()->setContextProperty("game", QVariant::fromValue(this));

    // Set up gametick timer
    m_timer.setInterval(100); // 10 times a second
    m_timer.setSingleShot(false);

    m_server.listen(QHostAddress::Any, 54321);

    connect(&m_timer, SIGNAL(timeout()), SLOT(gameTick()));
    connect(&m_server, SIGNAL(newConnection()), SLOT(clientConnect()));
    connect(this, SIGNAL(gameOver()), SLOT(endRound()));
}

GameManager::~GameManager()
{
    // Ensure we don't crash because we delete stuff before it disconnects
    foreach(NetworkClient *client, m_clients) {
        if (client) {
            disconnect(client, SIGNAL(clientDisconnected()), this, SLOT(clientDisconnected()));
        }
    }
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
        if (!m_clients[i]) continue; // Don't kick out human player

        m_players.takeAt(i)->deleteLater();
        m_clients.takeAt(i)->deleteLater();
    }

    // Update positions and id
    for (int i=0; i<playerCount(); i++) {
        player(i)->setPosition(m_map->startingPositions()[i]);
        player(i)->setId(i);
    }

    m_view->rootContext()->setContextProperty("players", QVariant::fromValue(m_players));
    m_view->rootContext()->setContextProperty("map", map);

    connect(m_map, SIGNAL(explosionAt(QPoint)), SLOT(explosionAt(QPoint)));
}

void GameManager::explosionAt(const QPoint &position)
{
    for (int i=0; i<playerCount(); i++) {
        if (player(i)->position() == position) {
            player(i)->setAlive(false);
        }
    }
}

void GameManager::endRound()
{
    m_timer.stop();

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
    }
}

void GameManager::startRound()
{
    if (m_players.isEmpty()) {
        return;
    }

    for (int i=0; i<playerCount(); i++) {
        player(i)->setAlive(true);
        player(i)->setPosition(m_map->startingPositions()[i]);
    }
    loadMap(m_map->name());

    // Do not allow to change name after game has started
    foreach(NetworkClient *client, m_clients) {
        client->disconnect(SIGNAL(nameChanged(QString)));
    }
    m_timer.start();
}

void GameManager::restartGame()
{
    m_roundsPlayed = 0;
    startRound();
}

void GameManager::gameTick()
{
    QList<Player*> players;
    for (int i=0; i<playerCount(); i++) {
        players.append(player(i));
    }
    for (int i=0; i<playerCount(); i++) {
        if (!m_clients[i]) continue;

        QList<Player*> list = players;
        list.takeAt(i);
        m_clients[i]->sendState(list, m_map, players[i]);
    }

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
            m_map->addBomb(position);
            continue;
        } else {
            continue;
        }

        if (m_map->isValidPosition(position)) {
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
        foreach(NetworkClient *client, m_clients) {
            if (client) client->sendOK();
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
        emit gameOver();
        return;
    }

    NetworkClient *client = qobject_cast<NetworkClient*>(sender());
    if (!client) {
        qWarning() << "GameManager: invalid sender for disconnect signal";
        return;
    }
    int index = m_clients.indexOf(client);
    if (index < 0) {
        qWarning() << "GameManager: unable to find disconnecting client.";
        return;
    }
    m_clients.takeAt(index)->deleteLater();
    m_players.takeAt(index)->deleteLater();
    for (int i=0; i<playerCount(); i++) {
        player(i)->setId(i);
    }
    // We need to reset the property, the qobjectlist model doesn't automatically update
    m_view->rootContext()->setContextProperty("players", QVariant::fromValue(m_players));
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

    Player *player = new Player(this, playerCount());
    player->setPosition(m_map->startingPositions().at(player->id()));
    m_players.append(player);
    m_clients.append(client);

    if (!client) {
        connect(m_view->rootObject(), SIGNAL(userMove(QString)), player, SLOT(setCommand(QString)));
        player->setName("Local user");
    } else {
        player->setName(client->remoteName());
        connect(client, SIGNAL(nameChanged(QString)), player, SLOT(setName(QString)));
        connect(client, SIGNAL(commandReceived(QString)), player, SLOT(setCommand(QString)));
        connect(client, SIGNAL(clientDisconnected()), SLOT(clientDisconnected()));
    }

    // We need to reset the property, the qobjectlist model doesn't automatically update
    m_view->rootContext()->setContextProperty("players", QVariant::fromValue(m_players));
}

void GameManager::removeHumanPlayers()
{
    for (int i=0; i<playerCount(); i++) {
        if (!m_clients[i]) {
            m_clients.takeAt(i)->deleteLater();
            m_players.takeAt(i)->deleteLater();
            break;
        }
    }
    for (int i=0; i<playerCount(); i++) {
        player(i)->setId(i);
    }
    m_view->rootContext()->setContextProperty("players", QVariant::fromValue(m_players));
}

QString GameManager::address()
{
    return m_server.serverAddress().toString() + QLatin1Char(':') + QString::number(m_server.serverPort());
}
