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

    enum Powerup {
        NoPowerup,
        NormalPellet,
        SuperPellet
    };
    Q_ENUM(Powerup)

    enum TileType {
        FloorTile = 0,
        WallTile,
        DoorTile,
        PelletTile,
        SuperPelletTile,
        InvalidTile = -1
    };
    Q_ENUM(TileType)

    explicit Map(QObject *parent);

    bool loadMap(const QString filepath);
    void resetPowerups();

    int width() const { return m_width; }
    int height() const { return m_height; }

    bool isNull();
    bool isValid();

    QList<QObject*> tiles();

    bool isValidPosition(int x, int y) const;
    bool isWithinBounds(int x, int y) const;

    QString name() const;

    Powerup takePowerup(int x, int y);
    Q_INVOKABLE QString tileSprite(int x, int y, TileCorner corner) const;
    Q_INVOKABLE QString powerupSprite(int x, int y) const;

    QVector<QPoint> startingPositions() { return m_startingPositions; }

signals:
    void mapChanged();
    void powerupChanged(int x, int y);

private:
    TileType tileAt(int x, int y) const;

    int m_width;
    int m_height;
    QVector<Powerup> m_powerups;
    QVector<TileType> m_tiles;
    QVector<QPoint> m_startingPositions;
    QString m_name;
};

#endif // MAP_H
