#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QTimer>
#include <QTcpServer>

class Map;
class QQuickView;
class Player;
class QQmlComponent;
class NetworkClient;

class GameManager : public QObject
{
    Q_OBJECT

public:
    explicit GameManager(QQuickView *parent);
    Q_INVOKABLE void loadMap(const QString &path);

    Player *player(int id);
    int playerCount();

    void addPlayer(NetworkClient *client = 0);

public slots:
    void addBomb(const QPoint &position);
    void explosionAt(const QPoint &position);
    void gameEnd();
    void gameStart();

signals:
    void gameOver();
    void clientConnected();

private slots:
    void gameTick();
    void clientConnect();

private:
    Map *m_map;
    QQuickView *m_view;
    QList<QObject*> m_players;
    QQmlComponent *m_bombComponent;
    QTimer m_timer;
    QTcpServer m_server;
};

#endif // MAPLOADER_H
