#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPoint>

class Player : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPoint position READ position() NOTIFY positionChanged())
    Q_PROPERTY(int id READ id() CONSTANT)
    Q_PROPERTY(bool alive READ isAlive() NOTIFY aliveChanged())
    Q_PROPERTY(QString lastCommand READ lastCommand() NOTIFY lastCommandChanged())

public:
    explicit Player(QObject *parent, int id);
    virtual ~Player() {}
    QPoint position() const;
    void setPosition(const QPoint &position);
    int id();
    void die();
    bool isAlive();

    QString lastCommand();
    QString command();

public slots:
    void setCommand(QString command);


signals:
    void positionChanged();
    void aliveChanged();
    void lastCommandChanged();

private:
    QPoint m_position;
    int m_id;
    bool m_alive;
    QString m_lastCommand;
    QString m_command;
};

#endif // PLAYER_H
