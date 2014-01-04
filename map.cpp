#include "map.h"


Map::Map(QObject *parent) : QObject(parent),
    m_width(0),
    m_height(0)
{
}

int Map::width()
{
    return m_width;
}

int Map::height()
{
    return m_height;
}

void Map::setWidth(int width)
{
    m_width = width; emit sizeChanged();
}

void Map::setHeight(int height)
{
    m_height = height;
    emit sizeChanged();
}

bool Map::isNull()
{
    return (m_width == 0 || m_height == 0);
}

bool Map::isValid()
{
    return (!isNull() && m_width * m_height == m_tiles.size());
}
