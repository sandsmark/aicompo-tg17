#include "networkclient.h"
#include <QTcpSocket>

NetworkClient::NetworkClient(QTcpSocket *socket) :
    QObject(socket), m_socket(socket)
{
}
