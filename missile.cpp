
#include "missile.h"
#include <QDebug>

Missile::Missile(QPointF startPosition, int startRotation, int owner, QObject *parent) : QObject(parent),
    m_position(startPosition),
    m_alive(true),
    m_owner(owner)
{
    m_rotation = (startRotation * M_PI * 2) / 360.0;
    m_velocityX = cos(m_rotation) * 0.05;
    m_velocityY = sin(m_rotation) * 0.05;
    m_energy = 1000;
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

    m_rotation = atan2(m_velocityY, m_velocityX);
    emit rotationChanged();


    if (m_energy > 50) {
        m_velocityX += cos(m_rotation) * (m_energy / 1000000.0);
        m_velocityY += sin(m_rotation) * (m_energy / 1000000.0);

        m_energy-= 50;
        emit energyChanged();
    }
    if (m_energy < 1) m_energy = 1;
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

    return missileObject;
}
