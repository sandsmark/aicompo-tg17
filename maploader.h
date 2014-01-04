#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <QObject>

class Map;
class QQmlContext;


class MapLoader : public QObject
{
    Q_OBJECT
public:
    explicit MapLoader(QQmlContext *parent);
    Q_INVOKABLE void loadMap(const QString &path);

private:
    Map *m_map;
    QQmlContext *m_context;
};

#endif // MAPLOADER_H
