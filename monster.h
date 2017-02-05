#ifndef MONSTER_H
#define MONSTER_H

#include <QObject>

class Map;

class Monster : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ getX NOTIFY positionChanged)
    Q_PROPERTY(int y READ getY NOTIFY positionChanged)
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
    Q_PROPERTY(QString direction READ getDirection NOTIFY directionChanged)

public:
    explicit Monster(QObject *parent = 0);

    int getX() { return m_x; }
    int getY() { return m_y; }
    QString getDirection() { return m_direction; }

    void setPosition(int getX, int getY);

    void setMap(Map *map);

    void doMove();

    bool isActive() { return m_active; }
    void setActive(bool active);

signals:
    void positionChanged();
    void activeChanged();
    void directionChanged();

public slots:

private:
    void verifyTarget();
    void findRandomTarget();

    Map *m_map;
    int m_x;
    int m_y;
    int m_rotation;
    bool m_active;
    QString m_direction;
    int m_targetX;
    int m_targetY;
};

#endif // MONSTER_H
