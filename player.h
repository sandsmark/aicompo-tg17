#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPoint>

class Player : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPoint position READ position() NOTIFY positionChanged())
    Q_PROPERTY(int id READ id() NOTIFY idChanged())
    Q_PROPERTY(bool alive READ isAlive() NOTIFY aliveChanged())
    Q_PROPERTY(QString lastCommand READ lastCommand() NOTIFY lastCommandChanged())
    Q_PROPERTY(QString name READ name() NOTIFY nameChanged())
    Q_PROPERTY(int wins READ wins() NOTIFY winsChanged())

public:
    explicit Player(QObject *parent, int id);
    virtual ~Player() {}
    QPoint position() const;
    void setPosition(const QPoint &position);
    int id();
    void setId(int id);
    void setAlive(bool alive);
    bool isAlive();

    QString lastCommand();
    QString command();

    QString name();

    void addWin() { m_wins++; emit winsChanged(); }
    int wins() { return m_wins; }

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

private:
    QPoint m_position;
    int m_id;
    bool m_alive;
    QString m_name;
    int m_wins;

    QString m_lastCommand;
    QString m_command;
};

#endif // PLAYER_H
