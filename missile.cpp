
#include "missile.h"
#include <QDebug>

Missile::Missile(Type type, QPointF startPosition, int startRotation, int owner, QObject *parent) : QObject(parent),
    m_type(type),
    m_position(startPosition),
    m_alive(true),
    m_owner(owner)
{
    if (type == Mine) {
        m_rotation = atan2(startPosition.y(), startPosition.x());

        m_velocityX = cos(m_rotation) * 0.005;
        m_velocityY = sin(m_rotation) * 0.005;
        m_energy = 5000;
    } else {
        m_rotation = (startRotation * M_PI * 2) / 360.0;

        m_velocityX = cos(m_rotation) * 0.05;
        m_velocityY = sin(m_rotation) * 0.05;

        m_energy = 1000;
    }


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


    // Just fall into the sun
    if (m_energy < 10) {
        m_velocityX /= 1.01;
        m_velocityY /= 1.01;
        return;
    }

    switch(m_type) {
    case Normal:
        m_rotation = atan2(m_velocityY, m_velocityX);
        emit rotationChanged();
        m_velocityX += cos(m_rotation) * (m_energy / 1000000.0);
        m_velocityY += sin(m_rotation) * (m_energy / 1000000.0);
        m_energy-= 50;
        emit energyChanged();
        break;
    case Mine:
        m_velocityX += cos(m_rotation) * 0.0005;
        m_velocityY += sin(m_rotation) * 0.0005;
        m_energy-= 1;
        emit energyChanged();
        break;
    }
}

QJsonObject Missile::serialize()
{
    QJsonObject missileObject;
    missileObject["owner"] =  m_owner;
    missileObject["x"] = m_position.x();
    missileObject["y"] = m_position.y();
    missileObject["velocityX"] = m_velocityX;
    missileObject["velocityY"] = m_velocityY;
    missileObject["rotation"] = m_rotation;
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
