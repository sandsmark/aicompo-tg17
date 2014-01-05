#include "networkclient.h"
#include <QTcpSocket>
#include <QHostAddress>
#include "player.h"

NetworkClient::NetworkClient(QTcpSocket *socket) :
    QObject(socket), m_socket(socket)
{
    socket->open(QIODevice::ReadWrite);
    connect(m_socket, SIGNAL(disconnected()), SIGNAL(clientDisconnected()));
    connect(m_socket, SIGNAL(readyRead()), SLOT(dataReceived()));
}

QString NetworkClient::remoteName()
{
    return m_socket->peerAddress().toString();
}

void NetworkClient::sendWelcome(const QByteArray &mapData, const QPoint &startData)
{
    m_socket->write("HELLO\n");
    m_socket->write("MAP\n");
    m_socket->write(mapData);
    m_socket->write("STARTPOSITION\n");
    m_socket->write(QByteArray::number(startData.x()) + ',');
    m_socket->write(QByteArray::number(startData.y()) + '\n');
}

void NetworkClient::sendOK()
{
    m_socket->write("OK\n");
}

void NetworkClient::sendPlayers(QList<Player *> players)
{
    m_socket->write("PLAYERS\n");
    foreach(Player *player, players) {
        if (!player) continue;
        m_socket->write(QByteArray::number(player->id()) + ' ');
        m_socket->write(QByteArray::number(player->position().x()) + ',');
        m_socket->write(QByteArray::number(player->position().y()) + '\n');
    }
}

void NetworkClient::dataReceived()
{
    QList<QByteArray> lines = m_socket->readAll().split('\n');
    qDebug() << lines;
    foreach(const QByteArray line, lines) {
        if (line.isEmpty()) continue;

        emit commandReceived(QString::fromLatin1(line.trimmed()));
    }
}
