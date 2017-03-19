#include "map.h"

#include <QDebug>
#include <QDir>
#include <QJsonArray>

Map::Map(QObject *parent) : QObject(parent),
    m_width(0),
    m_height(0),
    m_pelletsLeft(0),
    m_totalPellets(0)
{
    loadMap(":/maps/default.txt");
}

bool Map::loadMap(const QString &filepath)
{
    QFile file(filepath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Map: Unable to open map file:" << filepath;
        m_parseError = "Map: Unable to open map file: " + filepath;
        return false;
    }

    QVector<TileType> tiles;
    int width = -1;
    int height = 0;
    QVector<QPoint> startingPositions;
    QPoint monsterSpawn;
    int totalPellets = 0;
    while (!file.atEnd()) {
        QByteArray line = file.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        if (width == -1) {
            width = line.length();
        }

        if (line.length() > 0 && line.length() != width) {
            qWarning() << "Invalid map line at line" << height << "width:" << line.length() << line;
            m_parseError = "Invalid map line at line number " + QString::number(height) + " width: " + QString::number(line.length()) + "\nContents: " + line;
            return false;
        }
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
                totalPellets++;
                break;
            case 'o':
                tiles.append(SuperPelletTile);
                break;
            case '#':
                tiles.append(FloorTile);
                startingPositions.append(QPoint(x, height));
                break;
            case '@':
                tiles.append(FloorTile);
                monsterSpawn = QPoint(x, height);
                break;
            default:
                qWarning() << "Invalid tile: '" << tile << "'";
                m_parseError = QStringLiteral("Invalid tile '") + tile +  "' at line number " + QString::number(height) + ":\n " + line;
                return false;
            }
            x++;
        }
        height++;
    }

    // Map is properly parsed, now we can modify ourselves
    m_name = filepath;
    m_tiles.clear();
    m_startingPositions = startingPositions;
    m_totalPellets = totalPellets;
    m_monsterSpawn = monsterSpawn;

    m_width = width;
    m_height = height;
    m_tiles = tiles;

    emit mapChanged();
    emit totalPelletsChanged();

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
        }
    }

    return true;
}

QStringList Map::availableMaps()
{
    QDir dir(":/maps/");
    QStringList ret;
    for (const QFileInfo &file : dir.entryInfoList(QDir::Files)) {
        ret.append(file.absoluteFilePath());
    }
    return ret;
}

void Map::resetPowerups()
{
    if (m_pelletsLeft == m_totalPellets) {
        return;
    }

    qDebug() << "Resetting powerups";
    if (m_powerups.size() != m_tiles.size()) {
        m_powerups.resize(m_tiles.size());
    }

    m_pelletsLeft = 0;

    for (int y=0; y<m_height; y++) {
        for (int x=0; x<m_width; x++) {
            const int pos = y * m_width + x;

            Powerup type;
            switch(m_tiles[pos]) {
            case PelletTile:
                type = NormalPellet;
                m_pelletsLeft++;
                break;
            case SuperPelletTile:
                type = SuperPellet;
                break;
            default:
                continue;
            }

            if (type == m_powerups[pos]) {
                continue;
            }

            m_powerups[pos] = type;
            emit powerupVisibleChanged(x, y, true);
        }
    }
    emit pelletsLeftChanged();
}

bool Map::isValid() const
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

Map::Powerup Map::powerupAt(int x, int y) const
{
    if (!isWithinBounds(x, y)) {
        return NoPowerup;
    }
    const int pos = y * m_width + x;
    if (pos >= m_powerups.size()) {
        return NoPowerup;
    }
    return m_powerups[pos];
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
    emit powerupVisibleChanged(x, y, false);
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
    switch(tileAt(x, y)) {
    case PelletTile:
        return "pellet";
    case SuperPelletTile:
        return "superpellet";
    default:
        return QString();
    }
}

QJsonObject Map::getSerialized() const
{
    QJsonObject ret;
    ret["width"] = width();
    ret["height"] = height();
    ret["pelletsleft"] = pelletsLeft();

    QJsonArray mapData;
    for (int y=0; y<m_height; y++) {
        QString mapRow;
        mapRow.reserve(m_width);
        for (int x=0; x<m_width; x++) {
            const Powerup powerup = powerupAt(x, y);
            if (powerup == NormalPellet) {
                mapRow += '.';
                continue;
            } else if (powerup == SuperPellet) {
                mapRow += QLatin1Char('o');
                continue;
            }

            const TileType tileType = tileAt(x, y);
            if (tileType == WallTile) {
                mapRow += QLatin1Char('|');
            } else if (tileType == DoorTile) {
                mapRow += QLatin1Char('-');
            } else {
                mapRow += QLatin1Char('_');
            }
        }
        mapData.append(mapRow);
    }

    ret["content"] = mapData;

    return ret;
}
