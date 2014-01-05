#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>

class QTcpSocket;

class NetworkClient : public QObject
{
    Q_OBJECT
public:
    explicit NetworkClient(QTcpSocket *socket);

signals:
    void commandReceived(const QString command);

public slots:
private:
    QTcpSocket *m_socket;
};

#endif // NETWORKCLIENT_H
