#ifndef MAP_H
#define MAP_H

#include <QList>
#include <QObject>
#include <QPoint>

#include "tile.h"

class Player;
class Bomb;
class GameManager;

class Map : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int width READ width() CONSTANT)
    Q_PROPERTY(int height READ height() CONSTANT)
    Q_PROPERTY(QList<QObject*> tiles READ tiles() CONSTANT)
    Q_PROPERTY(QString name READ name() CONSTANT)
    Q_PROPERTY(int maxPlayers READ maxPlayers() CONSTANT)

public:
    explicit Map(GameManager *game, const QString &mapName);

    int width() const;
    int height() const;

    bool isNull();
    bool isValid();

    QList<QObject*> tiles();

    const QList<QPoint> &startingPositions() const;

    bool isValidPosition(const QPoint &position) const;
    bool isWithinBounds(const QPoint &position) const;

    QString name() const;

    int maxPlayers() const;

    void addBomb(const QPoint &position, Player *player);

    QList<Bomb*> bombs() const { return m_bombs; }

    Tile *tileAt(const QPoint &position) const;

public slots:
    void detonateBomb(const QPoint &position);

signals:
    void explosionAt(QPoint position);

private:
    bool explodeTile(const QPoint &position);

    int m_width;
    int m_height;
    QList<QObject*> m_tiles;
    QList<QPoint> m_startingPositions;
    QString m_name;
    QList<Bomb*> m_bombs;
    GameManager *m_game;
};

#endif // MAP_H
