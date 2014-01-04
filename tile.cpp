#include "tile.h"
#include <QDebug>

Tile::Tile(QObject *parent, Type type) :
    QObject(parent),
    m_type(type)
{
}

