#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QPointer>
#include <QList>
#include <QString>
#include <QTimer>
#include <QTcpServer>
#include <QSoundEffect>

#include "missile.h"
#include "player.h"
#include "parameters.h"

class QQuickView;
class QQmlComponent;
class NetworkClient;

class GameManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int roundsPlayed READ roundsPlayed() NOTIFY roundsPlayedChanged())
    Q_PROPERTY(bool soundEnabled READ soundEnabled WRITE setSoundEnabled NOTIFY soundEnabledChanged)
    Q_PROPERTY(int ticksLeft READ ticksLeft NOTIFY tick)
    Q_PROPERTY(QList<QObject*> players READ players NOTIFY playersChanged)
    Q_PROPERTY(int maxPlayers READ maxPlayerCount CONSTANT)
    Q_PROPERTY(int maxRounds READ maxRounds CONSTANT)

public:
    explicit GameManager(QQuickView *parent);
    ~GameManager();

    Q_INVOKABLE void removeHumanPlayers();

    Q_INVOKABLE void setTickInterval(int interval);

    QQuickView *view() { return m_view; }

    Q_INVOKABLE QString version();

    bool soundEnabled() { return m_soundEnabled; }
    void setSoundEnabled(bool enabled);
    int ticksLeft() const { return m_ticksLeft; }

    QList<QObject *> players() const;

    int maxPlayerCount() { return MAX_PLAYERS; }
    int maxRounds() { return MAX_ROUNDS; }

public slots:
    void explosionAt(const QPoint &position);
    void endRound();
    void startRound();
    void playBombSound();
    void stopGame();
    void togglePause();
    void addPlayer(NetworkClient *client = 0);
    void kick(int index);
    void resetScores();

    int roundsPlayed() { return m_roundsPlayed; }

signals:
    void roundOver();
    void clientConnected();
    void roundsPlayedChanged();
    void tick();
    void soundEnabledChanged();
    void playersChanged();
    void explosion(QPointF position);
    void missileCreated(QObject *missile);

private slots:
    void gameTick();
    void clientConnect();
    void clientDisconnected();

private:
    void resetPositions();
    QJsonObject serializeForPlayer(Player *player);
//    void createMissileSprite(Missile *parent);

    QQuickView *m_view;
    QList<Player*> m_players;
    QList<Missile*> m_missiles;
    QTimer m_tickTimer;
    QTcpServer m_server;
    int m_roundsPlayed;

    bool m_soundEnabled;
    QSoundEffect m_backgroundLoop;
    QVector<QSoundEffect*> m_explosions;
    QSoundEffect m_death;
    int m_ticksLeft;
};

#endif // GAMEMANAGER_H
