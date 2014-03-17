#ifndef BOMB_H
#define BOMB_H

#include <QObject>
#include <QPoint>
#include <QTimer>

class QQuickItem;
class QQuickView;

#define BOMB_STATES 14
#define BOMB_TIME 1000

class Bomb : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int state READ state() NOTIFY stateChanged())
    Q_PROPERTY(QPoint position READ position() CONSTANT)

public:
    explicit Bomb(QQuickView *view, QPoint position);
    ~Bomb();

    int state();
    QPoint position();

    void blow();

signals:
    void boom(const QPoint &position);
    void stateChanged();
    void aboutToBlow();

private slots:
    void tick();

private:
    QPoint m_position;
    QTimer m_timer;
    int m_state; // Goes 1-13, then boom on 14
    QQuickItem *m_sprite;
};

#endif // BOMB_H
