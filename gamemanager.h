#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QPointer>
#include <QList>
#include <QString>
#include <QTimer>
#include <QTcpServer>

#include "missile.h"
#include "player.h"
#include "parameters.h"

class QQuickView;
class QQmlComponent;
class NetworkClient;

class GameManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isGameRunning READ isGameRunning NOTIFY isGameRunningChanged)
    Q_PROPERTY(int roundsPlayed READ roundsPlayed() NOTIFY roundsPlayedChanged())
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

    bool isGameRunning() const { return m_isGameRunning; }
    int ticksLeft() const { return m_ticksLeft; }

    QList<QObject *> players() const;

    int maxPlayerCount() { return MAX_PLAYERS; }
    int maxRounds() { return MAX_ROUNDS; }

public slots:
    void endRound();
    void startRound();
    void stopGame();
    void togglePause();
    void addPlayer(NetworkClient *client = 0);
    void kick(int index);
    void resetScores();

    int roundsPlayed() { return m_roundsPlayed; }

signals:
    void isGameRunningChanged();
    void roundOver();
    void clientConnected();
    void roundsPlayedChanged();
    void tick();
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

    QQuickView *m_view;
    QList<Player*> m_players;
    QList<Missile*> m_missiles;
    QTimer m_tickTimer;
    QTcpServer m_server;
    int m_roundsPlayed;
    int m_ticksLeft;
    bool m_isGameRunning;
};

#endif // GAMEMANAGER_H
