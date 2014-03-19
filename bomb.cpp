#include "bomb.h"
#include <QDebug>
#include <QQuickItem>
#include <QQuickView>
#include <QTimer>
Bomb::Bomb(QObject *parent, QQuickView *view, QPoint position) :
    QObject(parent), m_position(position), m_state(0), m_sprite(0)
{
    static QQmlComponent *bombComponent = 0;


    if (!bombComponent) {
        bombComponent = new QQmlComponent(view->engine(), QUrl("qrc:/qml/bomb/BombSprite.qml"), QQmlComponent::PreferSynchronous);
    }
    if (bombComponent->status() != QQmlComponent::Ready) {
        qWarning() << "Bomb: Unable to instantiate bomb sprite:" << bombComponent->errorString();;
        return;
    }
    QObject *playingFieldObject = view->rootObject()->findChild<QObject*>("playingField");
    if (!playingFieldObject) {
        qWarning() << "Bomb: Unable to locate playing field object!";
        return;
    }
    QQuickItem *playingField = qobject_cast<QQuickItem*>(playingFieldObject);
    if (!playingField) {
        qWarning() << "Bomb: Unable to locate playing field QML item!";
        return;
    }

    m_sprite = qobject_cast<QQuickItem*>(bombComponent->create());
    m_sprite->setParentItem(playingField);

    m_sprite->setProperty("bombData", QVariant::fromValue(this));
    m_sprite->setProperty("bombX", QVariant::fromValue(position.x()));
    m_sprite->setProperty("bombY", QVariant::fromValue(position.y()));
}

Bomb::~Bomb()
{
    QTimer::singleShot(500, m_sprite, SLOT(deleteLater()));
}

void Bomb::tick()
{
    if (m_state == BOMB_STATES) {
        emit boom(m_position);
    } else if (m_state < BOMB_STATES) {
        m_state++;
    } else if (m_state == BOMB_STATES - 2) {
        emit aboutToBlow();
    }

    emit stateChanged();
}

void Bomb::blow()
{
    m_state = BOMB_STATES;
    tick();
}

int Bomb::state()
{
    return m_state;
}

QPoint Bomb::position() const
{
    return m_position;
}
