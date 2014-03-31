#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <QObject>
#include <QPointer>
#include <QList>
#include <QString>
#include <QTimer>
#include <QTcpServer>
#include <QSoundEffect>
#include "player.h"

class Map;
class QQuickView;
class QQmlComponent;
class NetworkClient;

class GameManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int roundsPlayed READ roundsPlayed() NOTIFY roundsPlayedChanged())
    Q_PROPERTY(QString address READ address() CONSTANT)
    Q_PROPERTY(QStringList maps READ maps() NOTIFY mapsChanged())
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)
    Q_PROPERTY(int ticksLeft READ ticksLeft NOTIFY tick)

public:
    explicit GameManager(QQuickView *parent);
    ~GameManager();
    Q_INVOKABLE void loadMap(const QString &path);

    Q_INVOKABLE void addPlayer(NetworkClient *client = 0);
    Q_INVOKABLE void removeHumanPlayers();

    Q_INVOKABLE void setDebugMode(bool debug) { m_tickTimer.setInterval(debug ? 80 : 100); qDebug() << m_tickTimer.interval(); }

    QString address();

    QQuickView *view() { return m_view; }

    QStringList maps();
    Q_INVOKABLE QString version() { return /*QLatin1String(APP_VERSION) + " // build time: " +*/ QLatin1String(__TIME__) + ' ' + QLatin1String(__DATE__); }
    bool soundEnabled() { return m_soundEnabled; }
    void setSoundEnabled(bool enabled);
    int ticksLeft() const { return m_ticksLeft; }

    QList<QPointer<Player> > players() const { return m_players; }

public slots:
    void explosionAt(const QPoint &position);
    void endRound();
    void startRound();
    void playBombSound();
    void stopGame();
    void togglePause();
    void kick(int index);
    void resetScores();

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
    void exportPlayerList();

    Map *m_map;
    QQuickView *m_view;
    QList<QPointer<Player> > m_players;
    QTimer m_tickTimer;
    QTcpServer m_server;
    QString m_currentMap;
    int m_roundsPlayed;

    bool m_soundEnabled;
    QSoundEffect m_backgroundLoop;
    QSoundEffect m_explosion;
    QSoundEffect m_death;
    int m_ticksLeft;
};

#endif // MAPLOADER_H
