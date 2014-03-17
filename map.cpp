#include "map.h"
#include "tile.h"
#include "player.h"
#include "gamemanager.h"
#include "bomb.h"

#include <QDebug>
#include <QDir>

Map::Map(GameManager *game, const QString &mapName) : QObject(game),
    m_width(0),
    m_height(0),
    m_name(mapName),
    m_game(game)
{
    QFile file(":/maps/" + mapName + ".map");

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Map: Unable to open file:" << mapName;
        return;
    }

    // Read in size
    QByteArray line = file.readLine();
    m_mapData.append(line);
    line = line.trimmed();
    m_height = line.split('x').first().toInt();
    m_width = line.split('x').last().toInt();
    if (height() == 0 || width() == 0) {
        qWarning() << "Map: Unable to parse size from file:" << mapName;
        return;
    }

    int players = file.readLine().trimmed().toInt();
    if (players < 2) {
        qWarning() << "Map: Unable to read a valid number of players for map:" << mapName;
        return;
    }

    for (int i = 0; i < players; i++) {
        line = file.readLine();
        QList<QByteArray> coordinates = line.trimmed().split(',');
        if (coordinates.size() != 2) {
            qWarning() << "Map: Unable to read starting coordinates for map:" << mapName;
            return;
        }

        QPoint position;
        position.setX(coordinates[0].toInt());
        position.setY(coordinates[1].toInt());
        m_startingPositions.append(position);
    }

    int c=0;
    // Read in the map itself
    while (!file.atEnd()) {
        c++;
        line = file.readLine();
        m_mapData.append(line);

        foreach(const char type, line) {
            switch(type) {
            case '.':
                m_tiles.append(new Tile(this, Tile::Grass));
                break;
            case '_':
                m_tiles.append(new Tile(this, Tile::Floor));
                break;
            case '+':
                m_tiles.append(new Tile(this, Tile::Wall));
                break;
            case '#':
                m_tiles.append(new Tile(this, Tile::Stone));
                break;
            }
        }
    }
}

int Map::width() const
{
    return m_width;
}

int Map::height() const
{
    return m_height;
}

bool Map::isValid()
{
    return (!m_tiles.isEmpty() && m_width * m_height == m_tiles.size());
}

QList<QObject *> Map::tiles() {
    return m_tiles;
}

bool Map::isValidPosition(const QPoint &position) const
{
    if (!isWithinBounds(position)) {
        return false;
    }

    Tile *tile = tileAt(position);

    if (!tile) {
        return false;
    }

    return tile->isWalkable();
}

bool Map::isWithinBounds(const QPoint &position) const
{
    if (position.x() < 0 || position.x() >= m_width)
        return false;

    if (position.y() < 0 || position.y() >= m_height)
        return false;

    return true;
}

QByteArray Map::mapData() const
{
    return m_mapData;
}

QString Map::name() const
{
    return m_name;
}

int Map::maxPlayers() const
{
    return m_startingPositions.size();
}

const QList<QPoint> &Map::startingPositions() const
{
    return m_startingPositions;
}

Tile *Map::tileAt(const QPoint &position) const
{
    int index = position.y() * m_width + position.x();
    if (index > m_tiles.size() || index < 0) {
        return NULL;
    }

    return qobject_cast<Tile*>(m_tiles[index]);
}

void Map::explodeTile(const QPoint &position)
{
    if (!isWithinBounds(position)) {
        return;
    }

    tileAt(position)->explode();

    foreach (Bomb *bomb, m_bombs) {
        if (bomb->position() == position) {
            bomb->blow();
        }
    }

    emit explosionAt(position);
}


void Map::addBomb(const QPoint &position)
{
    Bomb *bomb = new Bomb(m_game->view(), position);
    m_bombs.append(bomb);
    connect(bomb, SIGNAL(boom(QPoint)), SLOT(detonateBomb(QPoint)));
}

void Map::detonateBomb(const QPoint &center)
{
    m_bombs.removeAll(qobject_cast<Bomb*>(sender()));
    sender()->deleteLater();

    for (int i = -2; i<=2; i++) {
        explodeTile(QPoint(center.x() + i, center.y()));
        explodeTile(QPoint(center.x(), center.y() + i));
    }
}
