#include "monster.h"
#include "map.h"
#include <QDebug>
#include <queue>
#include <QSet>
#include <qmath.h>
#include "gamemanager.h"

Monster::Monster(QObject *parent) : QObject(parent),
    m_map(nullptr),
    m_x(0),
    m_y(0),
    m_active(true),
    m_direction("spawn")
{

}

void Monster::setPosition(int x, int y)
{
    m_x = x;
    m_y = y;
    m_targetX = -1;
    m_targetY = -1;
    emit positionChanged();
}

void Monster::setMap(Map *map)
{
    m_map = map;
    m_x = m_map->width() / 2;
    m_y = m_map->height() / 2;
}

struct PathPoint {
    PathPoint() {}
    PathPoint(int _x, int _y) : x(_x), y(_y) {}
    int x = 0;
    int y = 0;
    qreal pathLength = 0;
    qreal distance = 0;

    int origX = 0;
    int orgiY = 0;

    // Lower distance is better
    bool operator<(const PathPoint &other) const {
//        return pathLength + distance < other.pathLength + other.distance;
        return other.pathLength + other.distance < pathLength + distance;
    }
    bool operator==(const PathPoint &other) const {
        return x == other.x && y == other.y;
    }
};

uint qHash(const PathPoint &point) {
    return point.x * 1000 + point.y;
}

void Monster::verifyTarget()
{
    // Still valid
    if (m_map->powerupAt(m_targetX, m_targetY) != Map::NoPowerup) {
        return;
    }

    // Just hunt a random player when there's no more candy
    if (!m_map->pelletsLeft()) {
        // The players are always shuffled by the game manager
        QList<Player*> victims = GameManager::instance()->getPlayers();
        for (const Player *victim : victims) {
            if (victim->isAlive()) {
                m_targetX = victim->x();
                m_targetY = victim->y();
                return;
            }
        }

        // This sucks, find a random place to go
        if (m_targetX == -1 || m_targetY == -1) {
            findRandomTarget();
        }

        return;
    }

    // Find the closest bestest candy
    m_targetX = -1;
    m_targetY = -1;

    int closestDistance = -1;
    Map::Powerup bestPowerup = Map::NoPowerup;

    for (int x=0; x<m_map->width(); x++) {
        for (int y=0; y<m_map->height(); y++) {
            Map::Powerup newPowerup = m_map->powerupAt(x, y);
            if (newPowerup == Map::NoPowerup) {
                continue;
            }
            if (newPowerup < bestPowerup) {
                continue;
            }

            const int distance = qAbs(x - m_x) + qAbs(y - m_y);
            if (distance < closestDistance || closestDistance == -1 || newPowerup > bestPowerup) {
                m_targetX = x;
                m_targetY = y;
                closestDistance = distance;
                bestPowerup = newPowerup;
            }
        }
    }
}

void Monster::findRandomTarget()
{
    for (int i=0; i<1000; i++) {
        m_targetX = qrand() % m_map->width();
        m_targetY = qrand() % m_map->height();
        if (m_map->isValidPosition(m_targetX, m_targetY)) {
            return;
        }
    }
}

void Monster::doMove()
{
    if (!m_active) {
        return;
    }
    if (!m_map) {
        qWarning() << "Monster can't move without map!";
    }

    verifyTarget();

    if (m_targetX == -1) {
        qWarning() << "no target!";
        return;
    }

    PathPoint currentPosition(m_x, m_y);

    QHash<PathPoint, PathPoint> cameFrom;

    // STL is a steaming pile of shit
    std::priority_queue<PathPoint> queue;
    queue.push(currentPosition);

    QSet<PathPoint> visited;

    while (!queue.empty()) {
        const PathPoint current = queue.top();
        queue.pop();

        if (current.x == m_targetX && current.y == m_targetY) {
            break;
        }

        visited.insert(current);
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                // Don't travel diagonally
                if (dx && dy) {
                    continue;
                }

                const int nx = current.x + dx;
                const int ny = current.y + dy;

                if (!m_map->isValidPosition(nx, ny)) {
                    continue;
                }

                PathPoint neighbor(nx, ny);
                if (visited.contains(neighbor)) {
                    continue;
                }

                neighbor.pathLength = current.pathLength + 1;
                neighbor.distance = qAbs(nx - m_targetX) + qAbs(ny - m_targetY);

                if (cameFrom.contains(neighbor) && cameFrom[neighbor].pathLength < current.pathLength) {
                    continue;
                }
                cameFrom[neighbor] = current;

                queue.push(neighbor);
            }
        }
    }

    PathPoint pathPoint = PathPoint(m_targetX, m_targetY);

    if (!cameFrom.contains(pathPoint)) {
        qWarning() << "Can't find path to target" << m_targetX << m_targetY << "from" << m_x << m_y;

        // Try not to do this next time
        findRandomTarget();
        return;
    }

    while (cameFrom[pathPoint].x != m_x || cameFrom[pathPoint].y != m_y) {
        pathPoint = cameFrom[pathPoint];
        if (!cameFrom.contains(pathPoint)) {
            qWarning() << "Invalid path";
            return;
        }
    }

    int newX = pathPoint.x;
    int newY = pathPoint.y;

    if (!m_map->isValidPosition(newX, newY)) {
        qWarning() << "New position is not valid!";
        m_targetX = -1;
        m_targetY = -1;
        return;
    }

    if (newX > m_x) {
        m_direction = "right";
    } else if (newX < m_x) {
        m_direction = "left";
    } else if (newY > m_y) {
        m_direction = "down";
    } else if (newY < m_y) {
        m_direction = "up";
    }

    emit directionChanged();
    m_x = newX;
    m_y = newY;
    emit positionChanged();
    m_map->takePowerup(m_x, m_y);

    // Check if we need to find a new target next time
    if (m_targetX == m_x && m_targetY == m_y) {
        m_targetX = -1;
        m_targetY = -1;
    }
}

void Monster::setActive(bool active)
{
    m_active = active;
    emit activeChanged();
}
