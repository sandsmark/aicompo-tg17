#include "player.h"

#include "networkclient.h"
#include "parameters.h"

#include <qmath.h> // because windows sucks assss
#include <QDebug>

Player::Player(QObject *parent, int id, NetworkClient *networkClient) : QObject(parent),
    m_id(id),
    m_wins(0),
    m_disconnected(false),
    m_alive(true),
    m_energy(START_ENERGY),
    m_velocityX(0),
    m_velocityY(0),
    m_score(0),
    m_networkClient(networkClient)
{
    if (networkClient) {
        networkClient->setParent(this); // automatically delete

        connect(networkClient, &NetworkClient::nameChanged, this, &Player::setName);
        connect(networkClient, &NetworkClient::commandReceived, this, &Player::setCommand);
        connect(networkClient, &NetworkClient::clientDisconnected, this, &Player::onDisconnected);
        connect(networkClient, &NetworkClient::clientDisconnected, this, &Player::clientDisconnected);
    }

    m_spritePath = "qrc:/sprites/players/player" + QString::number(id) + ".png";
    emit spritePathChanged();
}

int Player::id()
{
    return m_id;
}

void Player::setId(int id)
{
    m_id = id;
    emit idChanged();

    m_spritePath = "qrc:/sprites/players/player" + QString::number(id) + ".png";
    emit spritePathChanged();
}

QString Player::lastCommand()
{
    return m_lastCommand;
}

QString Player::command()
{
    QString command = m_command;
    m_command.clear();

    if (m_lastCommand != command) {
        m_lastCommand = command;
        emit lastCommandChanged();
    }

    return command;
}

QString Player::name()
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

    if (command.startsWith("SAY ")) {
        m_message = command.remove(0, 4);
        emit messageReceived();
    } else {
        m_command = command;
    }
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
    }
}

bool Player::isAlive()
{
    if (m_disconnected) {
        return false;
    }

    return m_alive;
}

void Player::setRotation(int rotation)
{
    if (rotation < 0) {
        rotation += 360;
    }
    if (rotation > 360) {
        rotation -= 360;
    }

    m_rotation = rotation;
    emit rotationChanged();
}

void Player::setPosition(QPointF position)
{
    qreal velocityAngle = atan2(position.y(), position.x()) + M_PI_2;
    m_velocityX = cos(velocityAngle) / 35.0;
    m_velocityY = sin(velocityAngle) / 35.0;

    m_position = position;
    emit positionChanged();

    setRotation(velocityAngle * 360 / (M_PI * 2.0));
}

QJsonObject Player::serialize()
{
    QJsonObject playerObject;
    playerObject["id"] =  m_id;
    playerObject["x"] = m_position.x();
    playerObject["y"] = m_position.y();
    playerObject["velocityX"] = m_velocityX;
    playerObject["velocityY"] = m_velocityY;
    playerObject["rotation"] = m_rotation;
    playerObject["energy"] = m_energy;

    return playerObject;
}

void Player::decreaseEnergy(int amount)
{
    if (m_energy <= 0) {
        return;
    }

    m_energy -= amount;
    if (m_energy < 0) m_energy = 0;

    emit energyChanged();

    if (m_energy == 0 && m_alive) {
        setAlive(false);
    }
}

void Player::increaseEnergy(int amount)
{
    if (m_energy <= 0) {
        return;
    }

    m_energy += amount;
    emit energyChanged();
}

void Player::rotate(int amount)
{
    if (m_energy <= 0) {
        return;
    }

    decreaseEnergy(ROTATE_COST);
    setRotation(m_rotation + amount);
}

void Player::setEnergy(int energy)
{
    if (energy == m_energy) {
        return;
    }

    m_energy = energy;
    emit energyChanged();
}

void Player::accelerate()
{
    if (m_energy <= 0) {
        return;
    }

    qreal angle = m_rotation * M_PI * 2.0 / 360.0;
    m_velocityX += cos(angle) * ACCELERATION_FORCE;
    m_velocityY += sin(angle) * ACCELERATION_FORCE;
    decreaseEnergy(ACCELERATION_COST);
}

void Player::doMove()
{
    if (m_energy <= 0) {
        return;
    }

    qreal x = m_position.x();
    qreal y = m_position.y();

    const qreal angle = atan2(y, x);
    const qreal distance = hypot(x, y);

    if (distance < 0.1) {
        m_alive = false;
        emit aliveChanged();
        return;
    }

    const qreal force = distance / m_energy;
    m_velocityX -= cos(angle) * force;
    m_velocityY -= sin(angle) * force;

    if (m_velocityX > 0.05) m_velocityX = 0.05;
    if (m_velocityY > 0.05) m_velocityY = 0.05;
    if (m_velocityX < -0.05) m_velocityX = -0.05;
    if (m_velocityY < -0.05) m_velocityY = -0.05;

    x += m_velocityX;
    y += m_velocityY;

    if (x > 1.0) { x = -1.0; }
    if (y > 1.0) { y = -1.0; }
    if (x < -1.0) { x = 1.0; }
    if (y < -1.0) { y = 1.0; }

    m_position.setX(x);
    m_position.setY(y);
    emit positionChanged();
    emit velocityChanged();
}
