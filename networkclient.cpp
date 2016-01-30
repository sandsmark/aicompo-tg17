#include "networkclient.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <QCryptographicHash>
#include <QJsonDocument>
#include "player.h"
#include "bomb.h"

NetworkClient::NetworkClient(QTcpSocket *socket) :
    QObject(socket), m_socket(socket), m_isWebSocket(false), m_json(false)
{
    socket->open(QIODevice::ReadWrite);
    m_name = m_socket->peerAddress().toString();

    connect(m_socket, SIGNAL(disconnected()), SIGNAL(clientDisconnected()));
    connect(m_socket, SIGNAL(readyRead()), SLOT(dataReceived()));
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
    if (m_isWebSocket) {
        foreach(QByteArray line, string.split('\n')) {
            m_socket->write("\0");
            m_socket->write(line + "\r\n");
            m_socket->write("\xff");
        }
    } else {
        m_socket->write(string);
    }
}

void NetworkClient::sendDead()
{
    if (m_json) {
        sendString("{ \"type\": \"dead\" }\n");
    } else {
        sendString("DEAD\n");
    }
}

void NetworkClient::sendEndOfRound()
{
    if (m_json) {
        sendString("{ \"type\": \"round end\" }\n");
    } else {
        sendString("ENDOFROUND\n");
    }
}

void NetworkClient::sendState(const QJsonObject gameState)
{
    QJsonObject stateObject;
    stateObject["messagetype"] = "stateupdate";

    stateObject["gamestate"] = gameState;
    QJsonDocument packet(stateObject);
    sendString(packet.toJson());
}

void NetworkClient::dataReceived()
{
    QByteArray data = m_socket->readAll();
    if (m_isWebSocket) {
        if (m_webSocketFrame.isEmpty() && !data.startsWith('\0')) return; // invalid data

        m_webSocketFrame += data;

        if (data.endsWith('\xff')) {
            data = m_webSocketFrame;
            m_webSocketFrame.clear();
        } else {
            return;
        }
    }


    QList<QByteArray> lines = data.split('\n');
    foreach(const QByteArray line, lines) {
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

        if (line.startsWith("JSON")) {
            m_json = true;
            continue;
        }

        if (line.startsWith("Sec-WebSocket-Key:")) {
            QList<QByteArray> splitLine = line.split(':');
            if (splitLine.length() < 2) continue;
            QByteArray handshake = splitLine[1].trimmed() + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

            QCryptographicHash hash(QCryptographicHash::Sha1);
            hash.addData(handshake);
            handshake = hash.result().toBase64();
            m_socket->write("HTTP/1.1 101 Switching Protocols\r\n"
                            "Upgrade: websocket\r\n"
                            "Connection: Upgrade\r\n"
                            "Sec-WebSocket-Accept: " + handshake + "\r\n"
                            "Sec-WebSocket-Protocol: chat\r\n");
            continue;
        }

        emit commandReceived(QString::fromLatin1(line.trimmed()));
    }
}
