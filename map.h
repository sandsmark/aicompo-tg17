#ifndef MAP_H
#define MAP_H

#include <QVector>
#include <QObject>
#include <QPoint>
#include <QColor>

class Map : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int width READ width() NOTIFY mapChanged)
    Q_PROPERTY(int height READ height() NOTIFY mapChanged)
    Q_PROPERTY(QString name READ name() NOTIFY mapChanged)

public:
    enum TileCorner {
        UpperLeft,
        UpperRight,
        BottomLeft,
        BottomRight
    };
    Q_ENUM(TileCorner)

    enum TileType {
        Floor = 0,
        Wall,
        Door,
        Pellet,
        Superpellet,
        Invalid = -1
    };
    Q_ENUM(TileType)

    explicit Map(QObject *parent);

    bool loadMap(const QString filepath);

    int width() const { return m_width; }
    int height() const { return m_height; }

    bool isNull();
    bool isValid();

    QList<QObject*> tiles();

    const QList<QPoint> &startingPositions() const;

    bool isValidPosition(int x, int y) const;
    bool isWithinBounds(int x, int y) const;

    QString name() const;

    Q_INVOKABLE TileType tileAt(int x, int y) const;

    Q_INVOKABLE QString tileSprite(int x, int y, TileCorner corner) const;

    Q_INVOKABLE QColor colorAt(int x, int y) {
        TileType tile = tileAt(x,y);
        switch (tile) {
        case Floor :
            return Qt::blue;
        case Wall :
            return Qt::red;
        case Door :
            return Qt::green;
        case Pellet :
            return Qt::white;
        case Superpellet :
            return Qt::gray;
        default:
            return Qt::cyan;
        }
    }

signals:
    void mapChanged();

private:
    int m_width;
    int m_height;
    QVector<TileType> m_tiles;
    QList<QPoint> m_startingPositions;
    QString m_name;
};

#endif // MAP_H
