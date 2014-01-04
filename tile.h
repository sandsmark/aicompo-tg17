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
        Invalid = -1
    };

    explicit Tile(QObject *parent, Type type);
    Tile() : m_type(Invalid) {}


    Type type() const { return m_type; }
    void setType(const Type type) { m_type = type; emit typeChanged(); }


signals:
    void typeChanged();

public slots:

private:
    Tile(const Tile &other);

    Type m_type;
};

#endif // TILE_H
