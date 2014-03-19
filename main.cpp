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

    app.setOrganizationDomain("gathering.org");
    app.setApplicationName("Drop da Bass");
    qmlRegisterType<Tile>("com.iskrembilen.bomb", 1, 0, "Tile");
    QQuickView view;
    QObject::connect((QObject*)view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    GameManager manager(&view);
    Q_UNUSED(manager);
    view.setSource(QUrl("qrc:/qml/bomb/main.qml"));
    view.showMaximized();
    return app.exec();
}
