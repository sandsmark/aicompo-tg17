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

public:
    explicit Player(QObject *parent, int id);

    QPoint position() const;

    void setPosition(const QPoint &position);

    int id();

    void die();

    bool isAlive();

signals:
    void positionChanged();
    void aliveChanged();

public slots:

private:
    QPoint m_position;
    int m_id;
    bool m_alive;
};

#endif // PLAYER_H
