#include "map.h"
#include "tile.h"
#include "player.h"

#include <QDebug>
#include <QDir>

Map::Map(QObject *parent, const QString &path) : QObject(parent),
    m_width(0),
    m_height(0)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Map: Unable to open file:" << path;
        return;
    }

    // Read in size
    QByteArray dimensions = file.readLine().trimmed();
    setHeight(dimensions.split('x').first().toInt());
    setWidth(dimensions.split('x').last().toInt());
    if (height() == 0 || width() == 0) {
        qWarning() << "Map: Unable to parse size from file:" << path;
        return;
    }

    int players = file.readLine().trimmed().toInt();
    if (players < 2) {
        qWarning() << "Map: Unable to read a valid number of players for map:" << path;
        return;
    }

    for (int i = 0; i < players; i++) {
        QList<QByteArray> coordinates = file.readLine().trimmed().split(',');
        if (coordinates.size() != 2) {
            qWarning() << "Map: Unable to read starting coordinates for map:" << path;
            return;
        }

        QPoint position;
        position.setX(coordinates[0].toInt());
        position.setY(coordinates[1].toInt());
        m_startingPositions.append(position);
    }

    // Read in the map itself
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        foreach(const char type, line) {
            switch(type) {
            case '.':
                addTile(new Tile(this, Tile::Grass));
                break;
            case '_':
                addTile(new Tile(this, Tile::Floor));
                break;
            case '+':
                addTile(new Tile(this, Tile::Wall));
                break;
            case '#':
                addTile(new Tile(this, Tile::Stone));
                break;
            }
        }
    }
}

int Map::width()
{
    return m_width;
}

int Map::height()
{
    return m_height;
}

void Map::setWidth(int width)
{
    m_width = width; emit sizeChanged();
}

void Map::setHeight(int height)
{
    m_height = height;
    emit sizeChanged();
}

bool Map::isValid()
{
    return (!m_tiles.isEmpty() && m_width * m_height == m_tiles.size());
}

QList<QObject *> Map::tiles() {
    return m_tiles;
}

void Map::addTile(Tile *tile) {
    m_tiles.append(tile); emit tilesChanged();
}

bool Map::isWalkable(const QPoint &position)
{
    const int x = position.x();
    const int y = position.y();

    if (x < 0 || x > m_width)
        return false;

    if (y < 0 || y > m_height)
        return false;

    Tile *tile = qobject_cast<Tile*>(m_tiles[y * m_width + x]);

    if (!tile) {
        return false;
    }

    Tile::Type type = tile->type();

    switch(type) {
    case Tile::Grass:
    case Tile::Floor:
        return true;
    case Tile::Stone:
    case Tile::Wall:
    case Tile::Invalid:
        return false;
    }

    return false;
}

const QList<QPoint> &Map::startingPositions() const
{
    return m_startingPositions;
}

void Map::detonateBomb(const QPoint &position)
{
    for (int i = -2; i<2; i++) {

    }
}
