#include "tile.h"
#include <QDebug>

Tile::Tile(QObject *parent, Type type) :
    QObject(parent),
    m_type(type)
{
}



Tile::Type Tile::type() const {
    return m_type;
}

void Tile::setType(const Tile::Type type) {
    m_type = type; emit typeChanged();
}

bool Tile::isWalkable() const
{
    switch(m_type) {
    case Tile::Grass:
    case Tile::Floor:
    case Tile::Debris:
        return true;
    case Tile::Stone:
    case Tile::Wall:
    case Tile::Invalid:
        return false;
    }

    return false;
}

void Tile::explode()
{
    if (m_type == Tile::Stone) {
        setType(Tile::Debris);
    }
    emit exploded();
}
