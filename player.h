#ifndef PLAYER_H
#define PLAYER_H

#include <QUrl>
#include <QObject>
#include <QPointF>
#include <QJsonObject>

class NetworkClient;

class Player : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(int wins READ wins NOTIFY winsChanged)
    Q_PROPERTY(int points READ points NOTIFY pointsChanged)
    Q_PROPERTY(bool alive READ isAlive NOTIFY aliveChanged)
    Q_PROPERTY(QString direction READ direction NOTIFY directionChanged)
    Q_PROPERTY(int x READ x NOTIFY positionChanged)
    Q_PROPERTY(int y READ y NOTIFY positionChanged)

public:
    enum Power {
        NoPower,
        SuperPellet
    };

    explicit Player(QObject *parent, int id, NetworkClient *networkClient = 0);
    virtual ~Player() {}
    int id();
    void setId(int id);
    bool isDisconnected() { return m_disconnected; }

    bool hasCommand() const { return !m_command.isEmpty(); }

    QString lastCommand();
    QString getCommand();
    QString direction();
    void setPower(Power power);
    Power currentPower() const;

    QString name() const;

    void addWin() { m_wins++; emit winsChanged(); }
    int wins() const { return m_wins; }

    NetworkClient *networkClient() { return m_networkClient; }

    void addPoints(int points) { m_points += points; emit pointsChanged();}
    int points() const { return m_points; }
    int takePoints();
    void resetPlayer();

    void setAlive(bool alive);
    bool isAlive() const;

    int x() const { return m_x; }
    int y() const { return m_y; }
    void setPosition(int x, int y);

    QJsonObject serialize();

    // For sorting the player list
    static bool comparePlayers(Player *a, Player *b)
    {
        if (a->wins() != b->wins()) {
            return a->wins() > b->wins();
        }

        return a->points() > b->points();
    }

    void gameTick();

public slots:
    void setCommand(QString getCommand);
    void setName(QString name);
    bool isHuman() { return m_networkClient == 0; }

signals:
    void commandReceived();
    void lastCommandChanged();
    void idChanged();
    void nameChanged();
    void winsChanged();
    void clientDisconnected();
    void pointsChanged();
    void spritePathChanged();
    void aliveChanged();
    void directionChanged();
    void positionChanged();

private slots:
    void onDisconnected();

private:
    int m_id;
    QString m_name;
    int m_wins;

    QString m_lastCommand;
    QString m_command;
    QString m_direction;
    bool m_disconnected;

    bool m_alive;
    int m_x;
    int m_y;

    int m_points;

    NetworkClient *m_networkClient;
    Power m_power;
    int m_powerLeft;
};

#endif // PLAYER_H
