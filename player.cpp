#include "player.h"

Player::Player(QObject *parent, int id) :
    QObject(parent), m_id(id)
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
