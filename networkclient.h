#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>
#include <QPoint>
#include <QJsonObject>

class QTcpSocket;
class Player;
class Map;
class Bomb;

class NetworkClient : public QObject
{
    Q_OBJECT
public:
    explicit NetworkClient(QTcpSocket *socket);
    QString remoteName();
    void sendWelcome(const QJsonObject &mapData, const QJsonObject &playerdata);
    void sendState(const QJsonObject gameState);
    void sendEndOfRound();
    void sendStartOfRound();
    void sendDead();
    void kick();

signals:
    void commandReceived(const QString command);
    void clientDisconnected();
    void nameChanged(QString name);

private slots:
    void dataReceived();

private:
    void sendMessage(const QJsonObject &message);
    void sendString(QByteArray string);

    QTcpSocket *m_socket;
    QString m_name;
};

#endif // NETWORKCLIENT_H
