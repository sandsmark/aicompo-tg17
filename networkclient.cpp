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

    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkClient::clientDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkClient::dataReceived);
}

QString NetworkClient::remoteName()
{
    return m_name;
}

void NetworkClient::sendWelcome(const QJsonObject &mapData, const QJsonObject &playerdata)
{
    QJsonObject object;
    object["messagetype"] = QStringLiteral("welcome");
    object["you"] = playerdata;
    object["map"] = mapData;
    sendMessage(object);
}

void NetworkClient::kick()
{
    m_socket->disconnectFromHost();
}

void NetworkClient::sendDead()
{
    QJsonObject message;
    message["messagetype"] = QStringLiteral("dead");
    sendMessage(message);
}

void NetworkClient::sendEndOfRound()
{
    QJsonObject message;
    message["messagetype"] = QStringLiteral("endofround");
    sendMessage(message);
}

void NetworkClient::sendStartOfRound()
{
    QJsonObject message;
    message["messagetype"] = QStringLiteral("startofround");
    sendMessage(message);
}

void NetworkClient::sendState(const QJsonObject &gameState)
{
    QJsonObject message;
    message["messagetype"] = QStringLiteral("stateupdate");
    message["gamestate"] = gameState;
    sendMessage(message);
}

void NetworkClient::dataReceived()
{
    QByteArray data = m_socket->readAll();


    QList<QByteArray> lines = data.split('\n');
    for (const QByteArray line : lines) {
        if (line.isEmpty()) continue;

        static const QByteArray nameCommand("name ");
        if (line.toLower().startsWith(nameCommand)) {
            m_name = line.mid(nameCommand.length());
            if (m_name.length() > 10) {
                m_name = m_name.left(10);
            }
            if (m_name.isEmpty()) {
                static int dumbs = 0;
                m_name = QString("Invalid %1").arg(++dumbs);
                qWarning() << "REceived invalid name:" << nameCommand;
            }
            emit nameChanged(m_name);
            continue;
        }

        emit commandReceived(QString::fromUtf8(line.trimmed()).toLower());
    }
}

void NetworkClient::sendMessage(const QJsonObject &message)
{
    if (!m_socket->isOpen()) {
        return;
    }

    m_socket->write(QJsonDocument(message).toJson(QJsonDocument::Compact));
    m_socket->write("\n", 1);
}
