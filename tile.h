#ifndef TILE_H
#define TILE_H

#include <QObject>

class Tile : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Type type READ type() WRITE setType() NOTIFY typeChanged())
    Q_ENUMS(Type)

public:
    enum Type {
        Grass = 0,
        Floor,
        Wall,
        Stone,
        Debris,
        Invalid = -1
    };

    explicit Tile(QObject *parent, Type type);
    Tile() : m_type(Invalid) {}


    Type type() const;
    void setType(const Type type);

    bool isWalkable() const;

    void explode();

signals:
    void typeChanged();
    void exploded();

public slots:

private:
    Tile(const Tile &other);

    Type m_type;
};

#endif // TILE_H
