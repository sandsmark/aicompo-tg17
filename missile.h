#ifndef MISSILE_H
#define MISSILE_H

#include <qmath.h> // because windows sucks assss
#include <QObject>
#include <QPointF>
#include <QJsonObject>

class Missile : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPointF position READ position NOTIFY positionChanged)
    Q_PROPERTY(int rotation READ rotation NOTIFY rotationChanged)
    Q_PROPERTY(int energy READ energy NOTIFY energyChanged)
    Q_PROPERTY(bool alive READ isAlive NOTIFY aliveChanged)

public:
    enum Type {
        Normal,
        Mine,
        Seeking
    };

    explicit Missile(Type type, QPointF startPosition, int startRotation, int owner, QObject *parent = 0);


    QPointF position() { return m_position; }

    int rotation() { return (m_rotation * 360 / (M_PI * 2)); }
    void setRotation(qreal rotation);

    void doMove();
    int energy() { return m_energy; }

    bool isAlive() { return m_alive; }
    Type type() { return m_type; }

    Q_INVOKABLE int owner() { return m_owner; }

    QJsonObject serialize();

signals:
    void positionChanged();
    void rotationChanged();
    void energyChanged();
    void aliveChanged();

public slots:
private:
    Type m_type;
    QPointF m_position;
    qreal m_rotation;
    qreal m_velocityX;
    qreal m_velocityY;
    int m_energy;
    bool m_alive;
    int m_owner;
    int m_id;
};

#endif // MISSILE_H
