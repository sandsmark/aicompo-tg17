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
    // We have to process this manually because QCommandLineParser might crash without a QCoreApplication
    bool runHeadless = false;
    for (int i=0; i<argc; i++) {
        if (argv[i] == QStringLiteral("--headless")) {
            runHeadless = true;
            break;
        }
    }

    // If we run headless we want a QCoreApplication so it doesn't try to connect to an X server
    QCoreApplication *app;
    if (runHeadless) {
        app = new QCoreApplication(argc, argv);
    } else {
        QGuiApplication *guiApp = new QGuiApplication(argc, argv);
        app = guiApp;

        QFontDatabase::addApplicationFont(":/Aldrich_Regular.ttf");
        guiApp->setFont(QFont("Aldrich"));
    }

    // Install custom message handler for logging to a file
    qInstallMessageHandler(myMessageHandler);

    // Set up command line options
    QCommandLineOption autostartOption("start-at",
                                       "Automatically start the game after <players> (1 - 4) have connected, default " + QString::number(MAX_PLAYERS),
                                       "players", QString::number(MAX_PLAYERS));
    QCommandLineOption tickintervalOption(QStringList() << "tick-interval" << "i" ,
                                          "Set the tick interval to <ms> milliseconds (10 - 1000), default " + QString::number(DEFAULT_TICKINTERVAL),
                                          "ms", QString::number(DEFAULT_TICKINTERVAL));
    QCommandLineOption quitOnFinishOption("quit-on-finish", "Exit the game after playing all rounds");
    QCommandLineOption fullscreenOption("fullscreen" , "Start the game in fullscreen" );
    QCommandLineOption roundsOption("rounds", "Set the number of rounds to <rounds>, default " + QString::number(MAX_ROUNDS),
                                    "rounds", QString::number(MAX_ROUNDS));
    QCommandLineOption headlessOption("headless", "Run without showing the UI, e. g. for running on a server. This implies the start-at and quit-on-finish options.");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption(autostartOption);
    parser.addOption(tickintervalOption);
    parser.addOption(quitOnFinishOption);
    parser.addOption(fullscreenOption);
    parser.addOption(roundsOption);
    parser.addOption(headlessOption);

    parser.process(*app);

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

    // Set application name and domain mainly for QSettings
    app->setApplicationName("Turn On Me");
    app->setOrganizationDomain("gathering.org");

    GameManager manager;
    manager.setMaxRounds(rounds);

    if (parser.isSet(autostartOption) || runHeadless) {
        manager.setCountdownDuration(500);

        qDebug() << "Waiting for" << startAtPlayers << "players...";

        QObject::connect(&manager, &GameManager::playersChanged, [&]{

            if (manager.roundsPlayed() < manager.maxRounds()) {
                qDebug() << "Player" << manager.players().count() << "of" << startAtPlayers << "connected...";
            }

            if (manager.players().count() >= startAtPlayers) {
                qDebug() << "All players connected, starting game";
                manager.startGame();
            }
        });
    }

    manager.setTickInterval(tickInterval);

    if (parser.isSet(quitOnFinishOption) || runHeadless) {
        QObject::connect(&manager, &GameManager::roundsPlayedChanged, [&] {

            if (manager.roundsPlayed() > 0) {
                qDebug() << "Round" << manager.roundsPlayed() << "of" << rounds << "finished";
            }

            if (manager.roundsPlayed() >= rounds) {
                qDebug() << "Game over.";
                app->quit();
            }
        });
    }

    if (!runHeadless) {
        qmlRegisterSingletonType<Settings>("org.gathering.turnonme", 1, 0, "Settings", [](QQmlEngine *, QJSEngine*) -> QObject* {
            return new Settings;
        });

        QQuickView *view = new QQuickView; // we leak this on exit, yolo
        QObject::connect(view->engine(), &QQmlEngine::quit, app, &QGuiApplication::quit);
        view->setResizeMode(QQuickView::SizeRootObjectToView);

        view->rootContext()->setContextProperty("GameManager", QVariant::fromValue(&manager));
        view->setSource(QUrl("qrc:/qml/main.qml"));
        QObject::connect(view->rootObject(), SIGNAL(userMove(QString)), &manager, SIGNAL(humanMove(QString)));

        if (parser.isSet(fullscreenOption)) {
            view->showFullScreen();
        } else {
            view->show();
        }
        int ret = app->exec();
        delete view;
        delete app;
        return ret;
    } else {
        int ret = app->exec();
        delete app;
        return ret;
    }
}
