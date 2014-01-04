#include "player.h"

#include <QDebug>

Player::Player(QObject *parent, int id) :
    QObject(parent), m_id(id), m_alive(true)
{
}

QPoint Player::position() const
{
    return m_position;
}

void Player::setPosition(const QPoint &position)
{
    m_position = position;

    emit positionChanged();
}

int Player::id()
{
    return m_id;
}

void Player::die()
{
    qDebug() << m_id << "died";
    m_alive = false;
    emit aliveChanged();
}

bool Player::isAlive()
{
    return m_alive;
}
