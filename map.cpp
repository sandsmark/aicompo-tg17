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
    QFile file(mapName);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Map: Unable to open file:" << mapName;
        return;
    }
    // Read in tick count for a round
    m_maxTicks = file.readLine().trimmed().toInt();

    // Read in size
    QByteArray line = file.readLine();
    line = line.trimmed();
    m_height = line.split('x').first().toInt();
    m_width = line.split('x').last().toInt();
    if (height() == 0 || width() == 0) {
        qWarning() << "Map: Unable to parse size from file:" << mapName;
        return;
    }

    for (int y=0; y<m_height; y++) {
        line = file.readLine().trimmed();
        for (int x=0; x<m_width; x++) {
            switch(line[x]) {
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
            case 'p':
                m_startingPositions.append(QPoint(x, y));
                m_tiles.append(new Tile(this, Tile::Floor));
                break;
            default:
                qWarning() << "Map: Illegal tile type:" << line[x];
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

bool Map::explodeTile(const QPoint &position)
{
    if (!isWithinBounds(position)) {
        return false;
    }

    bool ret = isValidPosition(position);

    tileAt(position)->explode();

    foreach (Bomb *bomb, m_bombs) {
        if (bomb->position() == position) {
            bomb->blow();
        }
    }

    emit explosionAt(position);

    return ret;
}


void Map::addBomb(const QPoint &position, Player *player)
{
    if (player && !player->canBomb()) return;

    foreach (const Bomb *bomb, m_bombs) {
        if (bomb->position() == position) return;
    }

    Bomb *bomb = new Bomb(this, m_game->view(), position, player);
    m_bombs.append(bomb);
    connect(bomb, SIGNAL(boom(QPoint)), SLOT(detonateBomb(QPoint)));
    connect(bomb, SIGNAL(aboutToBlow()), m_game, SLOT(playBombSound()));
    connect(m_game, SIGNAL(tick()), bomb, SLOT(tick()));
}

void Map::detonateBomb(const QPoint &center)
{
    m_bombs.removeAll(qobject_cast<Bomb*>(sender()));
    sender()->deleteLater();


    explodeTile(QPoint(center.x(), center.y()));

    if (explodeTile(QPoint(center.x() + 1, center.y()))) {
        explodeTile(QPoint(center.x() + 2, center.y()));
    }
    if (explodeTile(QPoint(center.x() - 1, center.y()))) {
        explodeTile(QPoint(center.x() - 2, center.y()));
    }

    if (explodeTile(QPoint(center.x(), center.y() + 1))) {
        explodeTile(QPoint(center.x(), center.y() + 2));
    }
    if (explodeTile(QPoint(center.x(), center.y() - 1))) {
        explodeTile(QPoint(center.x(), center.y() - 2));
    }
}

void Map::explodeEverything()
{
    for (int x=0; x<width(); x++) {
        for (int y=0; y<height(); y++) {
            if (tileAt(QPoint(x, y))->type() == Tile::Stone) {
                explodeTile(QPoint(x, y));
            }
        }
    }
}

void Map::addRandomBomb()
{
    QPoint position;
    bool valid;
    for (int i=0; i<1000; i++) {
        valid = true;
        position.setX(qrand() % width());
        position.setY(qrand() % height());
        foreach(Bomb *bomb, m_bombs) {
            if (bomb->position() == position) {
                valid = false;
                break;
            }
        }
        if (!valid) continue;
        foreach(QPointer<Player> player, m_game->players()) {
            if (!player) continue;
            if (player->position() == position) {
                valid = false;
                break;
            }
        }
        if (valid) break;
    }
    if (!valid) return;
    addBomb(position, 0);
}
