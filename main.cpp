#include "gamemanager.h"
#include "tile.h"
#include "bomb.h"
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QtQml>
#include <QQuickItem>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Tile>("com.iskrembilen.bomb", 1, 0, "Tile");
    //qmlRegisterType<Bomb>("com.iskrembilen.bomb", 1, 0, "Bomb");

    //QtQuick2ApplicationViewer viewer;
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    GameManager *manager = new GameManager(&view);
    view.setSource(QStringLiteral("qml/bomb/main.qml"));
    view.showMaximized();
    QObject *item = view.rootObject();
    QObject::connect(item, SIGNAL(userMove(QString)), manager, SLOT(userMove(QString)));

    return app.exec();
}
