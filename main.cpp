#include "maploader.h"
#include "tile.h"
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QtQml>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Tile>("com.iskrembilen.bomb", 1, 0, "Tile");

    //QtQuick2ApplicationViewer viewer;
    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.rootContext()->setContextProperty("mapLoader", new MapLoader(view.rootContext()));
    view.setSource(QStringLiteral("qml/bomb/main.qml"));
    view.showMaximized();

    return app.exec();
}
