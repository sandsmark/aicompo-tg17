#include "missile.h"

#include "parameters.h"

#include <QDebug>


Missile::Missile(Type type, QPointF startPosition, int startRotation, int owner, QObject *parent) : QObject(parent),
    m_type(type),
    m_position(startPosition),
    m_alive(true),
    m_owner(owner)
{
    static int idCounter = 0;
    m_id = idCounter;
    idCounter++;

    if (type == Mine) {
        setRotation(atan2(startPosition.y(), startPosition.x()));

        m_velocityX = cos(m_rotation) * 0.005;
        m_velocityY = sin(m_rotation) * 0.005;
        m_energy = MINE_START_ENERGY;
        return;
    }

    setRotation((startRotation * M_PI * 2) / 360.0);

    if (type == Normal) {
        m_velocityX = cos(m_rotation) * 0.05;
        m_velocityY = sin(m_rotation) * 0.05;
    } else if (type == Seeking) {
        m_velocityX = cos(m_rotation) * 0.03;
        m_velocityY = sin(m_rotation) * 0.03;
    }

    m_energy = MISSILE_START_ENERGY;
}

void Missile::setRotation(qreal rotation)
{
    if (rotation < 0) {
        rotation += M_PI * 2.0;
    }

    if (rotation > M_PI * 2.0) {
        rotation -= M_PI * 2.0;
    }

    m_rotation = rotation;
    emit rotationChanged();
}

void Missile::doMove()
{
    qreal x = m_position.x();
    qreal y = m_position.y();
    const qreal distance = hypot(x, y);

    if (distance < 0.1) {
        m_alive = false;
        emit aliveChanged();
        return;
    }

    qreal velocityMagnitude = hypot(m_velocityX, m_velocityY);
    if (velocityMagnitude > MISSILE_MAX_SPEED) {
        qreal velocityAngle = atan2(m_velocityY, m_velocityX);
        m_velocityX = cos(velocityAngle) * MISSILE_MAX_SPEED;
        m_velocityY = sin(velocityAngle) * MISSILE_MAX_SPEED;
    }

    const qreal force = distance / 1000;
    const qreal angle = atan2(y, x);
    m_velocityX -= cos(angle) * force;
    m_velocityY -= sin(angle) * force;

    x += m_velocityX;
    y += m_velocityY;

    if (x > 1.0) { x = -1.0; }
    if (y > 1.0) { y = -1.0; }
    if (x < -1.0) { x = 1.0; }
    if (y < -1.0) { y = 1.0; }

    m_position.setX(x);
    m_position.setY(y);
    emit positionChanged();


    // Always point in the right direction
    if (m_type == Normal) {
        setRotation(atan2(m_velocityY, m_velocityX));
    }

    // Just fall into the sun
    if (m_energy < 10) {
        m_velocityX /= 1.01;
        m_velocityY /= 1.01;
        return;
    }

    if (m_type == Mine) {
        m_velocityX += cos(m_rotation) * 0.0005;
        m_velocityY += sin(m_rotation) * 0.0005;
        m_energy-= 1;
        emit energyChanged();
        return;
    }

    m_energy-= 50;

    emit energyChanged();

    if (m_type == Normal) {
        m_velocityX += cos(m_rotation) * (m_energy / 1000000.0);
        m_velocityY += sin(m_rotation) * (m_energy / 1000000.0);
    } else if (m_type == Seeking) {
        m_velocityX += cos(m_rotation) * (m_energy / 100000.0);
        m_velocityY += sin(m_rotation) * (m_energy / 100000.0);
    }
}

QJsonObject Missile::serialize()
{
    QJsonObject missileObject;
    missileObject["id"] = m_id;
    missileObject["owner"] =  m_owner;
    missileObject["x"] = m_position.x();
    missileObject["y"] = m_position.y();
    missileObject["velocityX"] = m_velocityX;
    missileObject["velocityY"] = m_velocityY;
    missileObject["rotation"] = rotation();
    missileObject["energy"] = m_energy;
    if (m_type == Mine) {
        missileObject["type"] = "MINE";
    } else if (m_type == Normal) {
        missileObject["type"] = "NORMAL";
    } else if (m_type == Seeking) {
        missileObject["type"] = "SEEKING";
    }

    return missileObject;
}
