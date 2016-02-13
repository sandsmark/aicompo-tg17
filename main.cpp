#include "gamemanager.h"
#include "settings.h"

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
        txt = QString("Debug: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtInfoMsg:
        txt = QString("Info: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
    break;
    case QtWarningMsg:
        txt = QString("Warning: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
    break;
    }
    QFile outFile("log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;

    std::cout << txt.toStdString() << std::endl;
}

#define ARGUMENT_TICK_INTERVAL "tick-interval"
#define ARGUMENT_START_AT "start-at"
#define ARGUMENT_TICK_INTERVAL "tick-interval"
#define ARGUMENT_QUIT_ON_FINISH "quit-on-finish"
#define ARGUMENT_FULLSCREEN "fullscreen"

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageHandler);
    QGuiApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption({ARGUMENT_START_AT, "Automatically start the game after <players> players (1 - 4) has connected.", "players"});
    parser.addOption({{"i", ARGUMENT_TICK_INTERVAL}, "Set the tick interval to <ms> milliseconds (10 - 1000).", "ms"});
    parser.addOption({ARGUMENT_QUIT_ON_FINISH, "Exit the game after playing all rounds."});
    parser.addOption({ARGUMENT_FULLSCREEN, "Start in fullscreen."});
    parser.process(app);

    QFontDatabase::addApplicationFont(":/Aldrich_Regular.ttf");
    app.setFont(QFont("Aldrich"));

    app.setOrganizationDomain("gathering.org");
    app.setApplicationName("Turn On Me");

    qmlRegisterSingletonType<Settings>("org.gathering.turnonme", 1, 0, "Settings", [](QQmlEngine *, QJSEngine*) -> QObject* {
        return new Settings;
    });


    QQuickView view;
    QObject::connect(view.engine(), &QQmlEngine::quit, &app, &QGuiApplication::quit);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    GameManager manager(&view);

    if (parser.isSet(ARGUMENT_START_AT)) {
        int startAtPlayers = parser.value(ARGUMENT_START_AT).toInt();
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

    if (parser.isSet(ARGUMENT_TICK_INTERVAL)) {
        int tickInterval = parser.value(ARGUMENT_TICK_INTERVAL).toInt();
        if (tickInterval < 10 || tickInterval > 1000) {
            parser.showHelp(-1);
        }
        manager.setTickInterval(tickInterval);
    }

    if (parser.isSet(ARGUMENT_QUIT_ON_FINISH)) {
        QObject::connect(&manager, &GameManager::roundsPlayedChanged, [&]{
            if (manager.roundsPlayed() >= MAX_ROUNDS) {
                app.quit();
            }
        });
    }

    view.setSource(QUrl("qrc:/qml/main.qml"));

    if (parser.isSet(ARGUMENT_FULLSCREEN)) {
        view.showFullScreen();
    } else {
        view.show();
    }

    return app.exec();
}
