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

    Q_PROPERTY(int id READ id() NOTIFY idChanged())
    Q_PROPERTY(QString lastCommand READ lastCommand() NOTIFY lastCommandChanged())
    Q_PROPERTY(QString name READ name() NOTIFY nameChanged())
    Q_PROPERTY(int wins READ wins() NOTIFY winsChanged())
    Q_PROPERTY(QString message READ message NOTIFY messageReceived())
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(QUrl spritePath MEMBER m_spritePath NOTIFY spritePathChanged())
    Q_PROPERTY(bool alive READ isAlive() NOTIFY aliveChanged())
    Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int energy READ energy NOTIFY energyChanged)
    Q_PROPERTY(int rotation READ rotation NOTIFY rotationChanged)
    Q_PROPERTY(qreal velocityX MEMBER m_velocityX NOTIFY velocityChanged)
    Q_PROPERTY(qreal velocityY MEMBER m_velocityY NOTIFY velocityChanged)

public:
    explicit Player(QObject *parent, int id, NetworkClient *networkClient = 0);
    virtual ~Player() {}
    int id();
    void setId(int id);
    bool isDisconnected() { return m_disconnected; }

    QString lastCommand();
    QString command();

    QString name();

    void addWin() { m_wins++; emit winsChanged(); }
    int wins() { return m_wins; }

    QString message() { return m_message; }
    void setMessage(QString message) { m_message = message; emit messageReceived(); }

    NetworkClient *networkClient() { return m_networkClient; }

    void addScore(int score) { m_score += score; emit scoreChanged();}
    int score() const { return m_score; }
    void resetScore() { m_score = 0; m_wins = 0; emit scoreChanged(); emit winsChanged(); }

    void doMove();

    void setAlive(bool alive);
    bool isAlive();

    void setRotation(int rotation);
    void rotate(int amount);
    int rotation() { return m_rotation; }

    int energy() { return m_energy; }
    void setEnergy(int energy);
    void decreaseEnergy(int amount);
    void increaseEnergy(int amount);
    void accelerate();

    QPointF position() const { return m_position; }
    void setPosition(QPointF position);

    void setVelocity(qreal vx, qreal vy);

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
    void messageReceived();
    void clientDisconnected();
    void scoreChanged();
    void spritePathChanged();
    void aliveChanged();
    void energyChanged();
    void positionChanged();
    void rotationChanged();
    void velocityChanged();

private slots:
    void onDisconnected();

private:
    int m_id;
    QString m_name;
    int m_wins;
    QString m_message;

    QString m_lastCommand;
    QString m_command;
    bool m_disconnected;
    QUrl m_spritePath;

    bool m_alive;
    int m_energy;
    int m_rotation;
    QPointF m_position;
    qreal m_velocityX;
    qreal m_velocityY;

    int m_score;

    NetworkClient *m_networkClient;
};

#endif // PLAYER_H
