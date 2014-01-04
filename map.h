#ifndef MAP_H
#define MAP_H

#include <QList>
#include <QObject>
#include <QPoint>

#include "tile.h"

class Player;

class Map : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int width READ width() NOTIFY sizeChanged())
    Q_PROPERTY(int height READ height() NOTIFY sizeChanged())
    Q_PROPERTY(QList<QObject*> tiles READ tiles() NOTIFY tilesChanged())


public:
    explicit Map(QObject *parent, const QString &filePath);

    int width();
    int height();
    void setWidth(int width);
    void setHeight(int height);

    bool isNull();
    bool isValid();

    QList<QObject*> tiles();
    void addTile(Tile *tile);

    bool isWalkable(const QPoint &position);

    const QList<QPoint> &startingPositions() const;

    void detonateBomb(const QPoint &position);

signals:
    void sizeChanged();
    void tilesChanged();

public slots:

private:
    int m_width;
    int m_height;
    QList<QObject*> m_tiles;
    QList<QPoint> m_startingPositions;
};

#endif // MAP_H
