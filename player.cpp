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


QString Player::lastCommand()
{
    return m_lastCommand;
}

QString Player::command()
{
    if (m_command.isEmpty()) {
        return m_command;
    }

    m_lastCommand = m_command;
    m_command.clear();

    emit lastCommandChanged();
    return m_lastCommand;
}

void Player::setCommand(QString command)
{
    m_command = command;
}
