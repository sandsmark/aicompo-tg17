#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPoint>

class Player : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPoint position READ position() NOTIFY positionChanged())
    Q_PROPERTY(int id READ id() CONSTANT)

public:
    explicit Player(QObject *parent, int id);

    QPoint position() const;

    void setPosition(const QPoint &position);

    int id();

signals:
    void positionChanged();

public slots:

private:
    QPoint m_position;
    int m_id;
};

#endif // PLAYER_H
