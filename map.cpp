#include "map.h"

#include <QDebug>
#include <QDir>

Map::Map(QObject *parent) : QObject(parent),
    m_width(0),
    m_height(0),
    m_pelletsLeft(0),
    m_totalPellets(0)
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
    m_totalPellets = 0;

    QVector<TileType> tiles;
    QByteArray line = file.readLine().trimmed();
    m_width = line.length();
    m_height = 0;
    while (!file.atEnd()) {
        int x = 0;
        for (const char tile : line) {
            switch(tile) {
            case '_':
                tiles.append(FloorTile);
                break;
            case '|':
                tiles.append(WallTile);
                break;
            case '-':
                tiles.append(DoorTile);
                break;
            case '.':
                tiles.append(PelletTile);
                m_totalPellets++;
                break;
            case 'o':
                tiles.append(SuperPelletTile);
                break;
            case '#':
                tiles.append(FloorTile);
                m_startingPositions.append(QPoint(x, m_height));
                break;
            default:
                qWarning() << "Invalid tile: '" << tile << "'";
                tiles.append(InvalidTile);
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

    resetPowerups();

    emit mapChanged();
    emit totalPelletsChanged();
    return true;
}

void Map::resetPowerups()
{
    m_powerups.clear();
    m_pelletsLeft = 0;
    for (int y=0; y<m_height; y++) {
        for (int x=0; x<m_width; x++) {
            switch(tileAt(x, y)) {
            case PelletTile:
                m_powerups.append(NormalPellet);
                m_pelletsLeft++;
                break;
            case SuperPelletTile:
                m_powerups.append(SuperPellet);
                break;
            default:
                m_powerups.append(NoPowerup);
            }
            emit powerupChanged(x, y);
        }
    }
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
    return (tile != WallTile && tile != InvalidTile);
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

Map::Powerup Map::takePowerup(int x, int y)
{
    if (!isWithinBounds(x, y)) {
        return NoPowerup;
    }
    const int pos = y * m_width + x;
    if (pos >= m_powerups.size()) {
        return NoPowerup;
    }
    Powerup powerup = m_powerups[pos];
    if (powerup == NoPowerup) {
        return powerup;
    }

    if (powerup == NormalPellet) {
        m_pelletsLeft--;
        emit pelletsLeftChanged();
    }

    m_powerups[pos] = NoPowerup;
    emit powerupChanged(x, y);
    return powerup;
}

Map::TileType Map::tileAt(int x, int y) const
{
    if (!isWithinBounds(x, y)) {
        return InvalidTile;
    }
    const int pos = y * m_width + x;
    if (pos >= m_tiles.length()) {
        return InvalidTile;
    }
    return m_tiles[pos];
}

QString Map::tileSprite(int x, int y, TileCorner corner) const
{
    TileType tile = tileAt(x,y);
    if (tile == DoorTile) {
        if (tileAt(x + 1, y) == WallTile || tileAt(x - 1, y) == WallTile) {
            return "door-horizontal";
        } else {
            return "door-vertical";
        }
    } else if (tile != WallTile) {
        return "floor";
    }

    const bool wallNorth = (tileAt(x, y - 1) == WallTile);
    const bool wallSouth = (tileAt(x, y + 1) == WallTile);
    const bool wallWest = (tileAt(x - 1, y) == WallTile);
    const bool wallEast = (tileAt(x + 1, y) == WallTile);
    const bool wallSouthEast = (tileAt(x + 1, y + 1) == WallTile);
    const bool wallSouthWest = (tileAt(x - 1, y + 1) == WallTile);
    const bool wallNorthEast = (tileAt(x + 1, y - 1) == WallTile);
    const bool wallNorthWest = (tileAt(x - 1, y - 1) == WallTile);

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

QString Map::powerupSprite(int x, int y) const
{
    if (!isWithinBounds(x, y)) {
        return QString("nopowerup");
    }
    const int pos = y * m_width + x;
    if (pos >= m_powerups.size()) {
        return QString("nopowerup");
    }
    switch(m_powerups[pos]) {
    case NoPowerup:
        return QString("nopowerup");
    case NormalPellet:
        return "pellet";
    case SuperPellet:
        return "superpellet";
    }
    return "invalid";
}
