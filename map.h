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

    bool isValidPosition(int x, int y) const;
    bool isWithinBounds(int x, int y) const;

    QString name() const;

    bool takePellet(int x, int y);
    Q_INVOKABLE bool tileHasPellet(int x, int y) const;
    Q_INVOKABLE TileType tileAt(int x, int y) const;

    Q_INVOKABLE QString tileSprite(int x, int y, TileCorner corner) const;

    QVector<QPoint> startingPositions() { return m_startingPositions; }

signals:
    void mapChanged();
    void pelletTaken(int x, int y);

private:
    int m_width;
    int m_height;
    QVector<bool> m_tileHasPellet;
    QVector<TileType> m_tiles;
    QVector<QPoint> m_startingPositions;
    QString m_name;
};

#endif // MAP_H
