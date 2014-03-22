#include "player.h"

#include "networkclient.h"

#include <QDebug>

Player::Player(QObject *parent, int id, NetworkClient *networkClient) :
    QObject(parent),
    m_id(id),
    m_wins(0),
    m_availableBombs(1),
    m_disconnected(false),
    m_lives(1),
    m_networkClient(networkClient)
{
    if (networkClient) {
        networkClient->setParent(this); // automatically delete

        connect(networkClient, SIGNAL(nameChanged(QString)), SLOT(setName(QString)));
        connect(networkClient, SIGNAL(commandReceived(QString)), SLOT(setCommand(QString)));
        connect(networkClient, SIGNAL(clientDisconnected()), SLOT(setDisconnected()));
        connect(networkClient, SIGNAL(clientDisconnected()), SIGNAL(clientDisconnected()));
    }
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

void Player::takeLife()
{
    m_lives--;

    if (m_lives > 0) {
        return;
    }

    if (m_networkClient) {
        m_networkClient->sendDead();
    }
    emit aliveChanged();
}

bool Player::isAlive()
{
    return (!m_disconnected && m_lives > 0);
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
    if (m_lives > 0) {
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
}


