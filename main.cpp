#include "gamemanager.h"
#include "tile.h"
#include "bomb.h"
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QtQml>
#include <QQuickItem>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);


    QFontDatabase::addApplicationFont(":/Aldrich_Regular.ttf");
    app.setFont(QFont("Aldrich"));

    app.setOrganizationDomain("gathering.org");
    app.setApplicationName("Turn On Me");
    qmlRegisterType<Tile>("com.iskrembilen.turnonme", 1, 0, "Tile");
    QQuickView view;
    QObject::connect((QObject*)view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    GameManager manager(&view);
    //if (app.arguments().contains("-auto")) {
    //    manager.addPlayer();
    //    manager.startRound();
    //}
    view.setSource(QUrl("qrc:/qml/main.qml"));
    view.showMaximized();
    return app.exec();
}
