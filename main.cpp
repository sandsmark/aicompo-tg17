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

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption({"start-at", "Automatically start the game after <players> players (1 - 4) has connected.", "players"});
    parser.addOption({{"i", "tick-interval"}, "Set the tick interval to <ms> milliseconds (10 - 1000).", "ms"});
    parser.addOption({"quit-on-finish", "Exit the game after playing all rounds."});
    parser.process(app);

    QFontDatabase::addApplicationFont(":/Aldrich_Regular.ttf");
    app.setFont(QFont("Aldrich"));

    app.setOrganizationDomain("gathering.org");
    app.setApplicationName("Turn On Me");
    QQuickView view;
    QObject::connect((QObject*)view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    GameManager manager(&view);

    if (parser.isSet("start-at")) {
        int startAtPlayers = parser.value("start-at").toInt();
        if (startAtPlayers < 1 || startAtPlayers > 4) {
            parser.showHelp(-1);
        }

        QObject::connect(&manager, &GameManager::playersChanged, [&]{
            if (manager.players().count() >= startAtPlayers) {
                manager.resetScores();
                manager.startRound();
            }
        });
    }

    if (parser.isSet("tick-interval")) {
        int tickInterval = parser.value("tick-interval").toInt();
        if (tickInterval < 10 || tickInterval > 1000) {
            parser.showHelp(-1);
        }
        manager.setTickInterval(tickInterval);
    }

    if (parser.isSet("quit-on-finish")) {
        QObject::connect(&manager, &GameManager::roundsPlayedChanged, [&]{
            if (manager.roundsPlayed() >= MAX_ROUNDS) {
                app.quit();
            }
        });
    }

    view.setSource(QUrl("qrc:/qml/main.qml"));
    view.showMaximized();
    //view.showFullScreen();
    return app.exec();
}
