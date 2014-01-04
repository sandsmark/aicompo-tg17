#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <QObject>
#include <QList>
#include <QString>

class Map;
class QQmlContext;
class Player;

class GameManager : public QObject
{
    Q_OBJECT

public:
    explicit GameManager(QQmlContext *parent);
    Q_INVOKABLE void loadMap(const QString &path);

    Player *player(int id);

public slots:
    void userMove(const QString &direction);

private:
    Map *m_map;
    QQmlContext *m_context;
    QList<QObject*> m_players;
};

#endif // MAPLOADER_H
