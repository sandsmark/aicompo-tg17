
#include "tile.h"
#include "map.h"
#include "player.h"

#include <QDebug>
#include <QFile>
#include <QQmlContext>
#include <QPoint>
#include <QList>

#include "gamemanager.h"

GameManager::GameManager(QQmlContext *view) : QObject(view),
    m_map(0), m_context(view)
{
    loadMap(":/maps/default.map");

    if (!m_map) {
        qWarning() << "GameManager: Unable to load default map!";
        return;
    }


    const QList<QPoint> startingPositions = m_map->startingPositions();
    for (int i=0; i < startingPositions.size(); i++) {
        m_players.append(new Player(this, i));
        player(i)->setPosition(startingPositions[i]);
    }

    m_context->setContextProperty("players", QVariant::fromValue(m_players));
}


void GameManager::loadMap(const QString &path)
{
    Map *map = new Map(this, path);

    if (!map->isValid()) {
        qWarning() << "Map: Parsed map isn't expected size";
        delete map;
        return;
    }

    m_context->setContextProperty("map", map);

    if (m_map)
        m_map->deleteLater();

    m_map = map;
}

void GameManager::userMove(const QString &direction)
{
    if (!m_map) return;

    Player *player = qobject_cast<Player*>(m_players[0]);

    QPoint position = player->position();

    if (direction == "up") {
        position.setY(position.y() - 1);
    } else if (direction == "down") {
        position.setY(position.y() + 1);
    } else if (direction == "left") {
        position.setX(position.x() - 1);
    } else if (direction == "right") {
        position.setX(position.x() + 1);
    } else {
        return;
    }

    if (m_map->isWalkable(position)) {
        player->setPosition(position);
    }
}

Player *GameManager::player(int id)
{
    if (id > m_players.size()) return NULL;
    return qobject_cast<Player*>(m_players[id]);
}
