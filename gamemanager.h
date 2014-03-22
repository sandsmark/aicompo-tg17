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
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)

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
    Q_INVOKABLE QString version() { return /*QLatin1String(APP_VERSION) + " // build time: " +*/ QLatin1String(__TIME__) + ' ' + QLatin1String(__DATE__); }
    bool soundEnabled() { return m_soundEnabled; }
    void setSoundEnabled(bool enabled);

public slots:
    void explosionAt(const QPoint &position);
    void endRound();
    void startRound();
    void playBombSound();
    void stopGame();
    void togglePause();

    int roundsPlayed() { return m_roundsPlayed; }

signals:
    void gameOver();
    void clientConnected();
    void roundsPlayedChanged();
    void mapsChanged();
    void tick();
    void soundEnabledChanged();

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

    bool m_soundEnabled;
    QSoundEffect m_backgroundLoop;
    QSoundEffect m_explosion;
    QSoundEffect m_death;
};

#endif // MAPLOADER_H
