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
    void nameChanged(QString name);

private slots:
    void dataReceived();

private:
    void sendString(QByteArray string);

    QTcpSocket *m_socket;
    QString m_name;
    bool m_isWebSocket;
    QByteArray m_webSocketFrame;
};

#endif // NETWORKCLIENT_H
