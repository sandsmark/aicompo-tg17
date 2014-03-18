#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QTimer>
#include <QTcpServer>
#include <QSoundEffect>

class Map;
class QQuickView;
class Player;
class QQmlComponent;
class NetworkClient;

class GameManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int roundsPlayed READ roundsPlayed() NOTIFY roundsPlayedChanged())
    Q_PROPERTY(QString address READ address() CONSTANT)
    Q_PROPERTY(QStringList maps READ maps() NOTIFY mapsChanged())

public:
    explicit GameManager(QQuickView *parent);
    ~GameManager();
    Q_INVOKABLE void loadMap(const QString &path);

    Player *player(int id);
    int playerCount();

    Q_INVOKABLE void addPlayer(NetworkClient *client = 0);
    Q_INVOKABLE void removeHumanPlayers();

    Q_INVOKABLE void setDebugMode(bool debug) { m_timer.setInterval(debug ? 1000 : 100); qDebug() << m_timer.interval(); }

    QString address();

    QQuickView *view() { return m_view; }

    QStringList maps();

public slots:
    void explosionAt(const QPoint &position);
    void endRound();
    void startRound();
    void restartGame();
    void playBombSound();

    int roundsPlayed() { return m_roundsPlayed; }

signals:
    void gameOver();
    void clientConnected();
    void roundsPlayedChanged();
    void mapsChanged();
    void tick();

private slots:
    void gameTick();
    void clientConnect();
    void clientDisconnected();

private:
    Map *m_map;
    QQuickView *m_view;
    QList<QObject*> m_players;
    QTimer m_timer;
    QTcpServer m_server;
    QList<NetworkClient*> m_clients;
    QString m_currentMap;
    int m_roundsPlayed;

    QSoundEffect m_backgroundLoop;
    QSoundEffect m_explosion;
    QSoundEffect m_death;
};

#endif // MAPLOADER_H
