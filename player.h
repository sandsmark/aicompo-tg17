#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPoint>
class NetworkClient;

class Player : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPoint position READ position() NOTIFY positionChanged())
    Q_PROPERTY(int id READ id() NOTIFY idChanged())
    Q_PROPERTY(bool alive READ isAlive() NOTIFY aliveChanged())
    Q_PROPERTY(QString lastCommand READ lastCommand() NOTIFY lastCommandChanged())
    Q_PROPERTY(QString name READ name() NOTIFY nameChanged())
    Q_PROPERTY(int wins READ wins() NOTIFY winsChanged())
    Q_PROPERTY(QString message READ message NOTIFY messageReceived())

public:
    explicit Player(QObject *parent, int id, NetworkClient *networkClient = 0);
    virtual ~Player() {}
    QPoint position() const;
    void setPosition(const QPoint &position);
    int id();
    void setId(int id);
    bool isAlive();

    QString lastCommand();
    QString command();

    QString name();

    void addWin() { m_wins++; emit winsChanged(); }
    int wins() { return m_wins; }

    void addActiveBomb() { m_availableBombs--; }
    void removeActiveBomb() { m_availableBombs++; }
    bool canBomb() { return m_availableBombs > 0; }

    QString message() { return m_message; }
    void setMessage(QString message) { m_message = message; emit messageReceived(); }

    NetworkClient *networkClient() { return m_networkClient; }
    void resetLives() { m_lives = 1; emit aliveChanged(); }
    void takeLife();
    void addLife() { m_lives++; }

public slots:
    void setCommand(QString command);
    void setName(QString name);

signals:
    void positionChanged();
    void aliveChanged();
    void lastCommandChanged();
    void idChanged();
    void nameChanged();
    void winsChanged();
    void messageReceived();
    void clientDisconnected();

private slots:
    void setDisconnected();

private:
    QPoint m_position;
    int m_id;
    QString m_name;
    int m_wins;
    QString m_message;

    QString m_lastCommand;
    QString m_command;
    int m_availableBombs;
    bool m_disconnected;
    int m_lives;

    NetworkClient *m_networkClient;
};

#endif // PLAYER_H
