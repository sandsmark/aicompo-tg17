#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "missile.h"
#include "player.h"
#include "parameters.h"

#include <QObject>
#include <QPointer>
#include <QList>
#include <QString>
#include <QTimer>
#include <QTcpServer>

#include <random>

class QQuickView;
class QQmlComponent;
class NetworkClient;

class GameManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool gameRunning READ isGameRunning NOTIFY gameRunningChanged)
    Q_PROPERTY(int roundsPlayed READ roundsPlayed() NOTIFY roundsPlayedChanged())
    Q_PROPERTY(QList<QObject*> players READ players NOTIFY playersChanged)
    Q_PROPERTY(int maxPlayers READ maxPlayerCount CONSTANT)
    Q_PROPERTY(int maxRounds READ maxRounds CONSTANT)

public:
    explicit GameManager();
    ~GameManager();

    Q_INVOKABLE void removeHumanPlayer();

    Q_INVOKABLE void setTickInterval(int interval);

    Q_INVOKABLE QString version();

    bool isGameRunning() const { return m_gameRunning; }

    QList<QObject *> players() const;

    int maxPlayerCount() { return MAX_PLAYERS; }
    int maxRounds() { return m_maxRounds; }
    void setMaxRounds(int maxRounds) { m_maxRounds = maxRounds; }

    void setCountdownDuration(int duration) { m_startTimer.setInterval(duration); }

public slots:
    void endRound();
    void startRound();
    void startGame();
    void stopGame();
    void togglePause();
    void addPlayer(NetworkClient *client = 0);
    void kick(int index);

    int roundsPlayed() { return m_roundsPlayed; }

signals:
    void gameRunningChanged();
    void roundOver();
    void clientConnected();
    void roundsPlayedChanged();
    void playersChanged();
    void explosion(QPointF position);
    void missileCreated(QObject *missile);
    void showCountdown();
    void humanMove(QString move);

private slots:
    void gameTick();
    void clientConnect();
    void clientDisconnected();

private:
    void resetPositions();
    QJsonObject serializeForPlayer(Player *player);

    QList<Player*> m_players;
    QList<Missile*> m_missiles;
    QTimer m_tickTimer;
    QTcpServer m_server;
    int m_roundsPlayed;
    bool m_gameRunning;
    QTimer m_startTimer;
    int m_maxRounds;
    std::random_device m_randomDevice;
    std::mt19937 m_randomGenerator;
};

#endif // GAMEMANAGER_H
