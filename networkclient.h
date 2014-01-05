#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QPoint>

class QTcpSocket;
class Player;

class NetworkClient : public QObject
{
    Q_OBJECT
public:
    explicit NetworkClient(QTcpSocket *socket);
    QString remoteName();
    void sendWelcome(const QByteArray &mapData, const QPoint &startData);
    void sendOK();
    void sendPlayers(QList<Player*> players);

signals:
    void commandReceived(const QString command);
    void clientDisconnected();

private slots:
    void dataReceived();

private:
    QTcpSocket *m_socket;
};

#endif // NETWORKCLIENT_H
