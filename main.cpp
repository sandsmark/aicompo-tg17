#include "gamemanager.h"
#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickView>
#include <QtQml>
#include <QQuickItem>
#include <QFontDatabase>
#include <iostream>

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtInfoMsg:
        txt = QString("Info: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(context.function);
    break;
    case QtWarningMsg:
        txt = QString("Warning: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(context.function);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(context.function);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1 (%2:%3, %4)\n").arg(msg).arg(context.file).arg(context.line).arg(context.function);
    break;
    }
    QFile outFile("log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;

    std::cout << txt.toStdString() << std::endl;
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageHandler);
    QGuiApplication app(argc, argv);


    QFontDatabase::addApplicationFont(":/Aldrich_Regular.ttf");
    app.setFont(QFont("Aldrich"));

    app.setOrganizationDomain("gathering.org");
    app.setApplicationName("Turn On Me");
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
    //view.showFullScreen();
    return app.exec();
}
