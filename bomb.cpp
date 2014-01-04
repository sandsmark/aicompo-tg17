#include "bomb.h"
#include <QDebug>
#include <QQuickItem>

Bomb::Bomb(QObject *parent, QPoint position, QQuickItem *sprite) :
    QObject(parent), m_position(position), m_state(0), m_sprite(sprite)
{
    m_timer.setInterval(BOMB_TIME / BOMB_STATES);
    m_timer.setSingleShot(false);
    connect(&m_timer, SIGNAL(timeout()), SLOT(tick()));
    m_timer.start();
}

Bomb::~Bomb()
{
    m_sprite->setParentItem(0);
    m_sprite->deleteLater();
}

void Bomb::tick()
{
    m_state++;
    if (m_state > BOMB_STATES) {
        emit boom(m_position);
        QMetaObject::invokeMethod(&m_timer, "stop");
    }

    emit stateChanged();
}

int Bomb::state()
{
    return m_state;
}

QPoint Bomb::position()
{
    return m_position;
}
