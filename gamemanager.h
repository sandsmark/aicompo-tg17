#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "player.h"
#include "parameters.h"
#include "map.h"
#include "monster.h"

#include <QElapsedTimer>
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
    Q_PROPERTY(bool roundRunning READ isRoundRunning NOTIFY roundRunningChanged)
    Q_PROPERTY(int roundsPlayed READ roundsPlayed() NOTIFY roundsPlayedChanged())
    Q_PROPERTY(QList<QObject*> players READ playerObjects NOTIFY playersChanged)
    Q_PROPERTY(int maxPlayers READ maxPlayerCount CONSTANT)
    Q_PROPERTY(int maxRounds READ maxRounds CONSTANT)
    Q_PROPERTY(QString timeElapsed READ timeElapsed NOTIFY secondsElapsedChanged)

public:
    static GameManager *instance();
    void terminate();

    Q_INVOKABLE void removeHumanPlayer();

    Q_INVOKABLE void setTickInterval(int interval);
    Q_INVOKABLE qreal getTickRate();

    Q_INVOKABLE QString version();

    Map *map() const { return m_map; }

    Q_INVOKABLE QObject *monster() { return m_monster; }

    bool isGameRunning() const { return m_gameRunning; }

    QList<QObject *> playerObjects() const;
    QList<Player *> getPlayers();

    int maxPlayerCount() { return MAX_PLAYERS; }
    int maxRounds() { return m_maxRounds; }
    void setMaxRounds(int maxRounds) { m_maxRounds = maxRounds; }

    void setCountdownDuration(int duration) { m_startTimer.setInterval(duration); }

    bool isRoundRunning() { return m_roundRunning; }

    void setTickless(bool tickless);

public slots:
    void endRound();
    void startRound();
    void startGame();
    void stopGame();
    void togglePause();
    void addPlayer(NetworkClient *client = 0);
    void kick(int index);

    int roundsPlayed() { return m_roundsPlayed; }
    QString timeElapsed();

signals:
    void gameRunningChanged();
    void gameStarted();
    void gameOver();

    void roundRunningChanged();
    void roundStarted();
    void roundOver();

    void clientConnected();
    void clientDisconnected();
    void roundsPlayedChanged();
    void playersChanged();
    void explosion(QPointF position);
    void showCountdown();
    void humanMove(QString move);
    void secondsElapsedChanged();

private slots:
    void gameTick();
    void onClientConnect();
    void onClientDisconnect();

private:
    explicit GameManager();

    void resetPositions();
    QJsonObject serializeForPlayer(Player *player);

    QList<Player*> m_players;
    QTimer m_tickTimer;
    QElapsedTimer m_elapsedTimer;
    QTcpServer m_server;
    int m_roundsPlayed;
    bool m_gameRunning;
    bool m_roundRunning;
    QTimer m_startTimer;
    int m_maxRounds;
    std::random_device m_randomDevice;
    std::mt19937 m_randomGenerator;
    Map *m_map;
    Monster *m_monster;
    bool m_tickless;
};



#endif // GAMEMANAGER_H
