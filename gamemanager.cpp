#include "gamemanager.h"

#include "tile.h"
#include "map.h"
#include "player.h"
#include "bomb.h"
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
    m_map(0), m_view(view), m_bombComponent(0)
{
    loadMap(":/maps/default.map");

    if (!m_map) {
        qWarning() << "GameManager: Unable to load default map!";
        return;
    }

    m_view->rootContext()->setContextProperty("players", QVariant::fromValue(m_players));
    m_view->rootContext()->setContextProperty("game", QVariant::fromValue(this));

    // Set up gametick timer
    // 20 times a second
    m_timer.setInterval(100);
    m_timer.setSingleShot(false);

    m_server.listen(QHostAddress::Any, 54321);

    connect(&m_timer, SIGNAL(timeout()), SLOT(gameTick()));
    connect(&m_server, SIGNAL(newConnection()), SLOT(clientConnect()));
    connect(m_map, SIGNAL(explosionAt(QPoint)), SLOT(explosionAt(QPoint)));
    connect(this, SIGNAL(gameOver()), SLOT(gameEnd()));
}

void GameManager::loadMap(const QString &path)
{
    Map *map = new Map(this, path);

    if (!map->isValid()) {
        qWarning() << "Map: Parsed map isn't expected size";
        delete map;
        return;
    }

    m_view->rootContext()->setContextProperty("map", map);

    if (m_map)
        m_map->deleteLater();

    m_map = map;
}

void GameManager::addBomb(const QPoint &position)
{
    if (!m_bombComponent) {
        m_bombComponent = new QQmlComponent(m_view->engine(), QUrl("qrc:/qml/bomb/BombSprite.qml"), QQmlComponent::PreferSynchronous);
    }

    if (m_bombComponent->status() != QQmlComponent::Ready) {
        qWarning() << "GameManager: Unable to instantiate bomb sprite:" << m_bombComponent->errorString();;
        return;
    }
    QObject *playingFieldObject = m_view->rootObject()->findChild<QObject*>("playingField");
    if (!playingFieldObject) {
        qWarning() << "GameManager: Unable to locate playing field object!";
        return;
    }
    QQuickItem *playingField = qobject_cast<QQuickItem*>(playingFieldObject);
    if (!playingField) {
        qWarning() << "GameManager: Unable to locate playing field QML item!";
        return;
    }

    QQuickItem *bombSprite = qobject_cast<QQuickItem*>(m_bombComponent->create());
    bombSprite->setParentItem(playingField);

    Bomb *bomb = new Bomb(this, position, bombSprite);
    connect(bomb, SIGNAL(boom(QPoint)), m_map, SLOT(detonateBomb(QPoint)));
    bombSprite->setProperty("bombData", QVariant::fromValue(bomb));
}

void GameManager::explosionAt(const QPoint &position)
{
    int dead = 0;
    foreach(QObject *obj, m_players) {
        Player *player = qobject_cast<Player*>(obj);
        if (player->position() == position) {
            player->die();
            dead++;
        }
    }
}

void GameManager::gameEnd()
{
    disconnect(m_map, SIGNAL(explosionAt(QPoint)), this, SLOT(explosionAt(QPoint)));
    disconnect(m_view->rootObject(), SIGNAL(userMove(QString)));

    QQuickItem *endScreen = qobject_cast<QQuickItem*>(m_view->rootObject()->findChild<QObject*>("endScreen"));
    if (endScreen) {
        endScreen->setProperty("opacity", 1.0);
    }
    m_timer.stop();
}

void GameManager::gameStart()
{
    if (m_players.isEmpty()) {
        return;
    }
    m_timer.start();
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
        m_clients[i]->sendPlayers(list);
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
            addBomb(position);
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

    if (m_players.size() - dead < 2) {
        emit gameOver();
    } else {
        foreach(NetworkClient *client, m_clients) {
            if (client) client->sendOK();
        }
    }
}

void GameManager::clientConnect()
{
    if (playerCount() >= m_map->startingPositions().count()) {
        return;
    }

    QTcpSocket *socket = m_server.nextPendingConnection();
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
    qDebug() << m_clients << client;
    int index = m_clients.indexOf(client);
    if (index < 0) {
        qDebug() << "GameManager: unable to find disconnecting client.";
        return;
    }
    m_clients.takeAt(index)->deleteLater();
    m_players.takeAt(index)->deleteLater();
    qDebug() << m_players;
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
        player->setName(QString::number(player->id()) + ". Local user");
    } else {
        client->sendWelcome(m_map->mapData(), player->position());
        player->setName(QString::number(player->id()) + ". " + client->remoteName());
        connect(client, SIGNAL(commandReceived(QString)), player, SLOT(setCommand(QString)));
        connect(client, SIGNAL(clientDisconnected()), SLOT(clientDisconnected()));
    }

    // We need to reset the property, the qobjectlist model doesn't automatically update
    m_view->rootContext()->setContextProperty("players", QVariant::fromValue(m_players));
}
