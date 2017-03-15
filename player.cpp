#include "player.h"

#include "networkclient.h"
#include "parameters.h"

#include <qmath.h> // because windows sucks assss
#include <QDebug>

Player::Player(QObject *parent, int id, NetworkClient *networkClient) : QObject(parent),
    m_id(id),
    m_wins(0),
    m_direction("down"),
    m_disconnected(false),
    m_alive(true),
    m_x(0),
    m_y(0),
    m_score(0),
    m_networkClient(networkClient),
    m_power(NoPower),
    m_powerLeft(0)
{
    if (networkClient) {
        networkClient->setParent(this); // automatically delete

        connect(networkClient, &NetworkClient::nameChanged, this, &Player::setName);
        connect(networkClient, &NetworkClient::commandReceived, this, &Player::setCommand);
        connect(networkClient, &NetworkClient::clientDisconnected, this, &Player::onDisconnected);
        connect(networkClient, &NetworkClient::clientDisconnected, this, &Player::clientDisconnected);
    }
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

QString Player::lastCommand()
{
    return m_lastCommand;
}

QString Player::getCommand()
{
    QString command = m_command;
    m_command.clear();

    if (m_lastCommand != command) {
        m_lastCommand = command;
        emit lastCommandChanged();
    }

    return command;
}

QString Player::direction()
{
    if (m_power == SuperPellet && (m_powerLeft > 50 || (m_powerLeft % 10 > 5))) {
        return "aggr";
    }
    return m_direction;
}

void Player::setPower(Player::Power power)
{
    if (power == SuperPellet) {
        m_powerLeft = SUPERPELLET_DURATION;
    }
    if (power == m_power) {
        return;
    }
    m_power = power;
    emit directionChanged();
}

Player::Power Player::currentPower() const
{
    if (m_powerLeft <= 0) {
        return NoPower;
    }

    return m_power;
}

QString Player::name() const
{
    return m_name;
}

void Player::setName(QString name)
{
    m_name = name;
    emit nameChanged();
}

void Player::setCommand(QString command)
{
    if (!isAlive()) {
        return;
    }

    static QStringList validCommands;
    if (validCommands.isEmpty()) {
        validCommands << "up" << "down" << "left" << "right";
    }
    if (!validCommands.contains(command)) {
        command = "";
    } else {
        m_direction = command;
        emit directionChanged();
    }

    m_command = command;

    emit commandReceived();
}

void Player::onDisconnected()
{
    m_disconnected = true;
    setAlive(false);
}

void Player::setAlive(bool alive)
{
    if (alive == m_alive) {
        return;
    }

    m_alive = alive;
    emit aliveChanged();

    if (!isAlive() && m_networkClient) {
        m_networkClient->sendDead();

        // To make sure it is handled correctly in tickless mode
        emit commandReceived();
    }
}

bool Player::isAlive() const
{
    if (m_disconnected) {
        return false;
    }

    return m_alive;
}

void Player::setPosition(int x, int y)
{
    if (x == m_x && y == m_y) {
        return;
    }

    m_x = x;
    m_y = y;
    emit positionChanged();
}

QJsonObject Player::serialize()
{
    QJsonObject playerObject;
    playerObject[QStringLiteral("id")] =  m_id;
    playerObject[QStringLiteral("x")] = m_x;
    playerObject[QStringLiteral("y")] = m_y;
    playerObject[QStringLiteral("score")] = m_score;
    playerObject[QStringLiteral("isdangerous")] = (m_power == SuperPellet);

    return playerObject;
}

void Player::gameTick()
{
    if (m_power != NoPower) {
        m_powerLeft--;
        if (m_powerLeft < 0) {
            setPower(NoPower);
        }
    }
}
