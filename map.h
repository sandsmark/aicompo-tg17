#ifndef MAP_H
#define MAP_H

#include <QList>
#include "tile.h"

class Map : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int width READ width() NOTIFY sizeChanged())
    Q_PROPERTY(int height READ height() NOTIFY sizeChanged())
    Q_PROPERTY(QList<QObject*> tiles READ tiles() NOTIFY tilesChanged())

public:
    explicit Map(QObject *);

    int width();
    int height();

    void setWidth(int width);
    void setHeight(int height);

    bool isNull();

    bool isValid();
    QList<QObject*> tiles() { return m_tiles; }

    void append(Tile *tile) { m_tiles.append(tile); emit tilesChanged(); }

signals:
    void sizeChanged();
    void tilesChanged();

public slots:

private:
    int m_width;
    int m_height;
    QList<QObject*> m_tiles;
};

#endif // MAP_H
