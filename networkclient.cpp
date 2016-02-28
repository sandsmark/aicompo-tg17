#include "networkclient.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QCryptographicHash>
#include <QJsonDocument>
#include "player.h"

NetworkClient::NetworkClient(QTcpSocket *socket) :
    QObject(socket), m_socket(socket)
{
    socket->open(QIODevice::ReadWrite);
    m_name = m_socket->peerAddress().toString();

    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkClient::clientDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkClient::dataReceived);
}

QString NetworkClient::remoteName()
{
    return m_name;
}

void NetworkClient::kick()
{
    m_socket->disconnectFromHost();
}

void NetworkClient::sendString(QByteArray string)
{
    if (!m_socket->isOpen()) {
        return;
    }

    m_socket->write(string + '\n');
}

void NetworkClient::sendDead()
{
    QJsonObject object;
    object["messagetype"] = "dead";
    QJsonDocument packet(object);
    sendString(packet.toJson(QJsonDocument::Compact));
}

void NetworkClient::sendEndOfRound()
{
    QJsonObject object;
    object["messagetype"] = "endofround";
    QJsonDocument packet(object);
    sendString(packet.toJson(QJsonDocument::Compact));
}

void NetworkClient::sendState(const QJsonObject gameState)
{
    QJsonObject stateObject;
    stateObject["messagetype"] = "stateupdate";

    stateObject["gamestate"] = gameState;
    QJsonDocument packet(stateObject);
    sendString(packet.toJson(QJsonDocument::Compact));
}

void NetworkClient::dataReceived()
{
    QByteArray data = m_socket->readAll();


    QList<QByteArray> lines = data.split('\n');
    for (const QByteArray line : lines) {
        if (line.isEmpty()) continue;

        if (line.startsWith("NAME ")) {
            QList<QByteArray> splitLine = line.split(' ');
            if (splitLine.length() < 2) continue;
            m_name = splitLine[1];
            if (m_name.length() > 10) {
                m_name = m_name.left(10);
            }
            emit nameChanged(m_name);
            continue;
        }

        emit commandReceived(QString::fromLatin1(line.trimmed()));
    }
}
