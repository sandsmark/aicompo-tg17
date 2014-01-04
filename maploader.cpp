#include "maploader.h"
#include "map.h"

#include <QFile>
#include <QDebug>
#include <QDir>
#include <QQmlContext>


MapLoader::MapLoader(QQmlContext *view) : QObject(view),
    m_map(0), m_context(view)
{
    loadMap(":/maps/default.map");
}


void MapLoader::loadMap(const QString &path)
{
    Map *map = new Map(this);
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Map: Unable to open file:" << path;
        return;
    }

    // Read in size
    QByteArray dimensions = file.readLine().trimmed();
    map->setHeight(dimensions.split('x').first().toInt());
    map->setWidth(dimensions.split('x').last().toInt());
    if (map->isNull()) {
        qWarning() << "Map: Unable to parse size from file:" << path;
        delete map;
        return;
    }

    // Read in the map itself
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        foreach(const char type, line) {
            switch(type) {
            case '.':
                map->append(new Tile(this, Tile::Grass));
                break;
            case '_':
                map->append(new Tile(this, Tile::Floor));
                break;
            case '+':
                map->append(new Tile(this, Tile::Wall));
                break;
            case '#':
                map->append(new Tile(this, Tile::Stone));
                break;
            }
        }
    }

    if (!map->isValid()) {
        qWarning() << "Map: Parsed map isn't expected size";
        delete map;
        return;
    }

    m_context->setContextProperty("map", map);
    m_context->setContextProperty("tiles", QVariant::fromValue(map->tiles()));

    if (m_map)
        m_map->deleteLater();

    m_map = map;
}
