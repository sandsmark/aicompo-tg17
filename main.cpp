#include "gamemanager.h"
#include "settings.h"
#include "parameters.h"

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
    case QtWarningMsg:
        txt = QString("Warning: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
        break;
    default:
        txt = QString("Info: %1 (%2:%3, %4)").arg(msg).arg(context.file).arg(context.line).arg(context.function);
        break;

    }
    QFile outFile("log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;

    std::cout << msg.toStdString() << std::endl;
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageHandler);

    QGuiApplication app(argc, argv);

    QFontDatabase::addApplicationFont(":/Aldrich_Regular.ttf");
    app.setFont(QFont("Aldrich"));

    app.setOrganizationDomain("gathering.org");
    app.setApplicationName("Turn On Me");


    QCommandLineParser parser;

    QCommandLineOption autostartOption("start-at", "Automatically start the game after <players> (1 - 4) have connected", "players", QString::number(MAX_PLAYERS));
    QCommandLineOption tickintervalOption(QStringList() << "tick-interval" << "i" ,
                                          "Set the tick interval to <ms> milliseconds (10 - 1000)", "ms", QString::number(DEFAULT_TICKINTERVAL));
    QCommandLineOption quitOnFinishOption("quit-on-finish", "Exit the game after playing all rounds");
    QCommandLineOption fullscreenOption("fullscreen" , "Start the game in fullscreen" );
    QCommandLineOption roundsOption("rounds", "Set the number of rounds to play to <rounds>", "rounds", QString::number(MAX_ROUNDS));

    parser.addHelpOption();
    parser.addOption(autostartOption);
    parser.addOption(tickintervalOption);
    parser.addOption(quitOnFinishOption);
    parser.addOption(fullscreenOption);
    parser.addOption(roundsOption);

    parser.process(app);

    int startAtPlayers = parser.value(autostartOption).toInt();
    if (startAtPlayers < 1 || startAtPlayers > MAX_PLAYERS) {
        qWarning() << "Invalid number of players:" << startAtPlayers;
        parser.showHelp(EINVAL);
    }

    int rounds = parser.value(roundsOption).toInt();
    if (rounds < 1) {
        qWarning() << "Invalid number of rounds:" << rounds;
        parser.showHelp(EINVAL);
    }

    int tickInterval = parser.value(tickintervalOption).toInt();
    if (tickInterval < 10 || tickInterval > 1000) {
        qWarning() << "Invalid tick interval:" << tickInterval;
        parser.showHelp(EINVAL);
    }

    qmlRegisterSingletonType<Settings>("org.gathering.turnonme", 1, 0, "Settings", [](QQmlEngine *, QJSEngine*) -> QObject* {
        return new Settings;
    });


    QQuickView view;
    QObject::connect(view.engine(), &QQmlEngine::quit, &app, &QGuiApplication::quit);
    view.setResizeMode(QQuickView::SizeRootObjectToView);

    GameManager manager(&view);
    manager.setMaxRounds(rounds);

    if (parser.isSet(autostartOption)) {
        manager.setCountdownDuration(0);

        QObject::connect(&manager, &GameManager::playersChanged, [&]{
            if (manager.players().count() >= startAtPlayers) {
                manager.startGame();
            }
        });
    }

    manager.setTickInterval(tickInterval);

    if (parser.isSet(quitOnFinishOption)) {
        QObject::connect(&manager, &GameManager::roundsPlayedChanged, [&]{
            if (manager.roundsPlayed() >= rounds) {
                app.quit();
            }
        });
    }

    view.setSource(QUrl("qrc:/qml/main.qml"));

    if (parser.isSet(fullscreenOption)) {
        view.showFullScreen();
    } else {
        view.show();
    }

    return app.exec();
}
