#include "map.h"

#include <QDebug>
#include <QDir>

Map::Map(QObject *parent) : QObject(parent),
    m_width(0),
    m_height(0)
{
}

bool Map::loadMap(const QString filepath)
{
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Map: Unable to open map file:" << filepath;
        return false;
    }

    m_name = file.fileName();
    m_tiles.clear();
    m_startingPositions.clear();

    QVector<TileType> tiles;
    QByteArray line = file.readLine().trimmed();
    m_width = line.length();
    m_height = 0;
    while (!file.atEnd()) {
        int x = 0;
        for (const char tile : line) {
            switch(tile) {
            case '_':
                tiles.append(Floor);
                break;
            case '|':
                tiles.append(Wall);
                break;
            case '-':
                tiles.append(Door);
                break;
            case '.':
                tiles.append(Pellet);
                break;
            case 'o':
                tiles.append(Superpellet);
                break;
            case '#':
                tiles.append(Floor);
                m_startingPositions.append(QPoint(x, m_height));
                break;
            default:
                qWarning() << "Invalid tile: '" << tile << "'";
                tiles.append(Invalid);
                break;
            }
            x++;
        }
        m_height++;
        line = file.readLine().trimmed();
        if (line.length() > 0 && line.length() != m_width) {
            qWarning() << "Invalid map line at line" << m_height << "width:" << line.length() << line;
            return false;
        }
    }

    m_tiles = tiles;
    emit mapChanged();
    return true;
}

bool Map::isValid()
{
    return (!m_tiles.isEmpty() && m_width * m_height == m_tiles.size());
}

bool Map::isValidPosition(int x, int y) const
{
    if (!isWithinBounds(x, y)) {
        return false;
    }

    TileType tile = tileAt(x, y);
    return (tile != Wall && tile != Invalid);
}

bool Map::isWithinBounds(int x, int y) const
{
    if (x < 0 || x >= m_width)
        return false;

    if (y < 0 || y >= m_height)
        return false;

    return true;
}

QString Map::name() const
{
    return m_name;
}

Map::TileType Map::tileAt(int x, int y) const
{
    if (!isWithinBounds(x, y)) {
        return Invalid;
    }
    const int pos = y * m_width + x;
    if (pos >= m_tiles.length()) {
        return Invalid;
    }
    return m_tiles[pos];
}

QString Map::tileSprite(int x, int y, TileCorner corner) const
{
    TileType tile = tileAt(x,y);
    if (tile == Door) {
        if (tileAt(x + 1, y) == Wall || tileAt(x - 1, y) == Wall) {
            return "door-horizontal";
        } else {
            return "door-vertical";
        }
    } else if (tile != Wall) {
        return "floor";
    }

    const bool wallNorth = (tileAt(x, y - 1) == Wall);
    const bool wallSouth = (tileAt(x, y + 1) == Wall);
    const bool wallWest = (tileAt(x - 1, y) == Wall);
    const bool wallEast = (tileAt(x + 1, y) == Wall);
    const bool wallSouthEast = (tileAt(x + 1, y + 1) == Wall);
    const bool wallSouthWest = (tileAt(x - 1, y + 1) == Wall);
    const bool wallNorthEast = (tileAt(x + 1, y - 1) == Wall);
    const bool wallNorthWest = (tileAt(x - 1, y - 1) == Wall);

    switch (corner) {
    case UpperLeft:
        if (!wallNorth && !wallWest && !wallNorthWest) {
            return "wall-corner";
        } else if (!wallNorth && wallWest) {
            return "wall-horizontal";
        } else if (wallNorth && !wallWest) {
            return "wall-vertical";
        } else if (wallWest && wallNorth && !wallNorthWest) {
            return "wall-corner-inverted";
        } else if (wallNorth && wallWest && wallNorthWest) {
            return "floor";
        }
        break;
    case UpperRight:
        if (!wallNorth && !wallEast && !wallNorthEast) {
            return "wall-corner";
        } else if (!wallNorth && wallEast) {
            return "wall-horizontal";
        } else if (wallNorth && !wallEast) {
            return "wall-vertical";
        } else if (wallEast && wallNorth && !wallNorthEast) {
            return "wall-corner-inverted";
        } else if (wallNorth && wallEast && wallNorthEast) {
            return "floor";
        }
        break;
    case BottomLeft:
        if (!wallSouth && !wallWest && !wallSouthWest) {
            return "wall-corner";
        } else if (!wallSouth && wallWest) {
            return "wall-horizontal";
        } else if (wallSouth && !wallWest) {
            return "wall-vertical";
        } else if (wallWest && wallSouth && !wallSouthWest) {
            return "wall-corner-inverted";
        } else if (wallSouth && wallWest && wallSouthWest) {
            return "floor";
        }
        break;
    case BottomRight:
        if (!wallSouth && !wallEast && !wallSouthEast) {
            return "wall-corner";
        } else if (!wallSouth && wallEast) {
            return "wall-horizontal";
        } else if (wallSouth && !wallEast) {
            return "wall-vertical";
        } else if (wallEast && wallSouth && !wallSouthEast) {
            return "wall-corner-inverted";
        } else if (wallSouth && wallEast && wallSouthEast) {
            return "floor";
        }
        break;
    }
    return "invalid";
}
