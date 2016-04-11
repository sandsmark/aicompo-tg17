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
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(bool alive READ isAlive NOTIFY aliveChanged)
    Q_PROPERTY(QString direction READ direction NOTIFY directionChanged)
    Q_PROPERTY(int x READ x NOTIFY positionChanged)
    Q_PROPERTY(int y READ y NOTIFY positionChanged)

public:
    explicit Player(QObject *parent, int id, NetworkClient *networkClient = 0);
    virtual ~Player() {}
    int id();
    void setId(int id);
    bool isDisconnected() { return m_disconnected; }

    QString lastCommand();
    QString command();
    QString direction() { return m_direction; }

    QString name();

    void addWin() { m_wins++; emit winsChanged(); }
    int wins() { return m_wins; }

    NetworkClient *networkClient() { return m_networkClient; }

    void addScore(int score) { m_score += score; emit scoreChanged();}
    int score() const { return m_score; }
    void resetScore() { m_score = 0; m_wins = 0; emit scoreChanged(); emit winsChanged(); }

    void setAlive(bool alive);
    bool isAlive();

    int x() { return m_x; }
    int y() { return m_y; }
    void setPosition(int x, int y);

    QJsonObject serialize();

    // For sorting the player list
    static bool comparePlayers(Player *a, Player *b)
    {
        if (a->wins() != b->wins()) {
            return a->wins() > b->wins();
        }

        return a->score() > b->score();
    }

public slots:
    void setCommand(QString command);
    void setName(QString name);
    bool isHuman() { return m_networkClient == 0; }

signals:
    void lastCommandChanged();
    void idChanged();
    void nameChanged();
    void winsChanged();
    void clientDisconnected();
    void scoreChanged();
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

    int m_score;

    NetworkClient *m_networkClient;
};

#endif // PLAYER_H
