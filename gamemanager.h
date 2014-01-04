#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <QObject>
#include <QList>
#include <QString>

class Map;
class QQuickView;
class Player;
class QQmlComponent;

class GameManager : public QObject
{
    Q_OBJECT

public:
    explicit GameManager(QQuickView *parent);
    Q_INVOKABLE void loadMap(const QString &path);

    Player *player(int id);

public slots:
    void userMove(const QString &direction);
    void addBomb(const QPoint &position);
    void explosionAt(const QPoint &position);
    void gameEnd();
    void gameStart();

signals:
    void gameOver();

private:
    Map *m_map;
    QQuickView *m_view;
    QList<QObject*> m_players;
    QQmlComponent *m_bombComponent;
};

#endif // MAPLOADER_H
