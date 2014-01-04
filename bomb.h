#ifndef BOMB_H
#define BOMB_H

#include <QObject>
#include <QPoint>
#include <QTimer>

class QQuickItem;

#define BOMB_STATES 14
#define BOMB_TIME 3000

class Bomb : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int state READ state() NOTIFY stateChanged())
    Q_PROPERTY(QPoint position READ position() CONSTANT)

public:
    explicit Bomb(QObject *parent, QPoint position, QQuickItem *sprite);
    ~Bomb();

    int state();
    QPoint position();

signals:
    void boom(const QPoint &position);
    void stateChanged();

private slots:
    void tick();

private:
    QPoint m_position;
    QTimer m_timer;
    int m_state; // Goes 1-13, then boom on 14
    QQuickItem *m_sprite;
};

#endif // BOMB_H
