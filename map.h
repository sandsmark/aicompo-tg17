#ifndef MAP_H
#define MAP_H

#include <QVector>
#include <QObject>
#include <QPoint>
#include <QColor>
#include <QJsonObject>
#include <QUrl>

class Map : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int width READ width() NOTIFY mapChanged)
    Q_PROPERTY(int height READ height() NOTIFY mapChanged)
    Q_PROPERTY(QString name READ name() NOTIFY mapChanged)
    Q_PROPERTY(int pelletsLeft READ pelletsLeft NOTIFY pelletsLeftChanged)
    Q_PROPERTY(int totalPellets MEMBER m_totalPellets NOTIFY totalPelletsChanged)

public:
    enum TileCorner {
        UpperLeft,
        UpperRight,
        BottomLeft,
        BottomRight
    };
    Q_ENUM(TileCorner)

    enum Powerup {
        NoPowerup = 0,
        NormalPellet = 1,
        SuperPellet = 2
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

    Q_INVOKABLE bool loadMap(const QUrl &fileUrl) { return loadMap(fileUrl.toLocalFile()); }
    Q_INVOKABLE bool loadMap(const QString &filepath);
    Q_INVOKABLE QString parseError() const { return m_parseError; }

    Q_INVOKABLE QStringList availableMaps();
    void resetPowerups();

    int width() const { return m_width; }
    int height() const { return m_height; }

    bool isNull() const;
    bool isValid() const;

    bool isValidPosition(int x, int y) const;
    bool isWithinBounds(int x, int y) const;

    QString name() const;

    Q_INVOKABLE Powerup powerupAt(int x, int y) const;
    Powerup takePowerup(int x, int y);
    Q_INVOKABLE QString tileSprite(int x, int y, TileCorner corner) const;
    Q_INVOKABLE QString powerupSprite(int x, int y) const;

    int pelletsLeft() const { return m_pelletsLeft; }

    QVector<QPoint> startingPositions() const { return m_startingPositions; }
    QPoint monsterSpawn() const { return m_monsterSpawn; }

    QJsonObject getSerialized() const;

signals:
    void mapChanged();
    void powerupChanged(int x, int y);
    void powerupVisibleChanged(int x, int y, bool visible);
    void pelletsLeftChanged();
    void totalPelletsChanged();

private:
    TileType tileAt(int x, int y) const;

    int m_width;
    int m_height;
    QVector<Powerup> m_powerups;
    QVector<TileType> m_tiles;
    QVector<QPoint> m_startingPositions;
    QPoint m_monsterSpawn;
    QString m_name;
    int m_pelletsLeft;
    int m_totalPellets;
    int m_arenaTop;
    int m_arenaBottom;
    QString m_parseError;
};

#endif // MAP_H
