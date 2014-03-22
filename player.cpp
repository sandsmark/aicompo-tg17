#include "player.h"

#include <QDebug>

Player::Player(QObject *parent, int id) :
    QObject(parent), m_id(id), m_alive(true), m_wins(0), m_availableBombs(1), m_disconnected(false)
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

void Player::setId(int id)
{
    m_id = id;
    emit idChanged();
}

void Player::setAlive(bool alive)
{
    if (m_disconnected) {
        alive = false;
    }

    m_alive = alive;
    emit aliveChanged();
}

bool Player::isAlive()
{
    if (m_disconnected) {
        return false;
    }
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

QString Player::name()
{
    return m_name + " (" + QString::number(m_id) + ')';
}

void Player::setName(QString name)
{
    m_name = name;
    emit nameChanged();
}

void Player::setCommand(QString command)
{
    if (m_alive) {
        if (command.startsWith("SAY ")) {
            m_message = command.remove(0, 4);
            emit messageReceived();
        } else {
            m_command = command;
        }
    }
}

void Player::setDisconnected()
{
    m_disconnected = true;
    setAlive(false);
}
