#include "gamemanager.h"

#include "bomb.h"
#include "tile.h"
#include "map.h"
#include "player.h"
#include "networkclient.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QQmlContext>
#include <QQuickView>
#include <QQuickItem>
#include <QPoint>
#include <QList>
#include <QQmlComponent>
#include <QTcpSocket>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QTimer>

#include <cmath>

#define VOLUME 0.5f

GameManager::GameManager(QQuickView *view) : QObject(view),
    m_map(0), m_view(view), m_roundsPlayed(0), m_ticksLeft(-1)
{
    loadMap(":/maps/default.map");

    if (!m_map) {
        qWarning() << "GameManager: Unable to load default map!";
        return;
    }

    // Add QML objects
    m_view->rootContext()->setContextProperty("GameManager", QVariant::fromValue(this));

    // Set up gametick timer
    m_tickTimer.setInterval(TICKLENGTH);
    m_tickTimer.setSingleShot(false);

    m_server.listen(QHostAddress::Any, 54321);

    connect(&m_tickTimer, SIGNAL(timeout()), SLOT(gameTick()));
    connect(&m_tickTimer, SIGNAL(timeout()), SIGNAL(tick()));
    connect(&m_server, SIGNAL(newConnection()), SLOT(clientConnect()));
    connect(this, SIGNAL(roundOver()), SLOT(endRound()));

    // Watch filesystem for map changes
    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    watcher->addPath("maps/");
    connect(watcher, SIGNAL(directoryChanged(QString)), SIGNAL(mapsChanged()));

    // Set up sound effects
    for(int i=0; i<8; i++) {
        QSoundEffect *explosion = new QSoundEffect(this);
        //explosion->setSource(QUrl::fromLocalFile("sound/explosion0" + QString::number(i+1) + ".wav"));
        explosion->setVolume(VOLUME);
        m_explosions.append(explosion);
    }
    m_backgroundLoop.setSource(QUrl::fromLocalFile("/home/sandsmark/src/turnonme/loop.wav"));
    m_backgroundLoop.setVolume(VOLUME);
    m_backgroundLoop.setLoopCount(QSoundEffect::Infinite);
    //m_death.setSource(QUrl::fromLocalFile("sound/death.wav"));
    m_death.setVolume(VOLUME);

    QSettings setting("sound");
    setSoundEnabled(setting.value("enabled", false).toBool());

//    QTimer::singleShot(250, this, SLOT(addPlayer()));
    QTimer::singleShot(250, this, SLOT(addPlayer()));
    QTimer::singleShot(250, this, SLOT(addPlayer()));
    QTimer::singleShot(100, this, SLOT(addPlayer()));
    QTimer::singleShot(200, this, SLOT(startRound()));
}

GameManager::~GameManager()
{
    // Ensure we don't crash because we delete stuff before it disconnects
    for (int i=0; i<m_players.count(); i++) {
        if (m_players[i]->networkClient()) {
            disconnect(m_players[i]->networkClient(), SIGNAL(clientDisconnected()), this, SLOT(clientDisconnected()));
        }
    }
}

void GameManager::setSoundEnabled(bool enabled)
{
    if (enabled == m_soundEnabled) return;

    QSettings setting("sound");
    setting.setValue("enabled", enabled);

    if (enabled) {
        m_backgroundLoop.play();
    } else {
        m_backgroundLoop.stop();
        for(int i=0; i<8; i++) {
            m_explosions[i]->stop();
        }
        m_death.stop();
    }
    m_soundEnabled = enabled;

    emit soundEnabledChanged();
}

QList<QObject*> GameManager::players() const
{
    QList<QObject*> playerList;
    foreach(Player *playerObject, m_players) {
        playerList.append(playerObject);
    }

    return playerList;
}

QList<QObject *> GameManager::missiles() const
{
    QList<QObject*> missileList;
    foreach(Missile *missileObject, m_missiles) {
        missileList.append(missileObject);
    }

    return missileList;
}

QStringList GameManager::maps()
{
    QStringList ret;

    ret << ":/maps/default.map"
        << ":/maps/Arena.map"
        << ":/maps/HugeArena.map";

    QDir mapDir("maps/");
    foreach (const QString file, mapDir.entryList(QDir::Files)) {
        ret << "maps/" + file;
    }

    return ret;
}

QString GameManager::version()
{
    QString versionString;

#ifdef APP_VERSION
    versionString += QStringLiteral(APP_VERSION);
#endif

    versionString += " // build time: " + QLatin1String(__TIME__) + ' ' + QLatin1String(__DATE__);
    return versionString;
}

void GameManager::loadMap(const QString &path)
{
    Map *map = new Map(this, path);

    if (!map->isValid()) {
        qWarning() << "Map: Parsed map isn't expected size";
        delete map;
        return;
    }

    if (m_map)
        m_map->deleteLater();
    m_map = map;


    // Kick out players if too many
    for (int i=m_players.count()-1; i>=0; --i) {
        if (m_players.count() <= m_map->startingPositions().count()) {
            break;
        }
        if (!m_players[i]->networkClient()) continue; // Don't kick out human player

        m_players.takeAt(i)->deleteLater();
    }


    // Get a randomized list of unoccupied starting positions
    QList<QPoint> startPositions = m_map->startingPositions();
    for (int index = startPositions.count() - 1; index > 0; --index) {
        qSwap(startPositions[index], startPositions[qrand() % (index + 1)]);
    }

    // Update positions and id
    for (int i=0; i<m_players.count(); i++) {
        //m_players[i]->setPosition(startPositions[i]);
        m_players[i]->setId(i);
    }

    connect(m_map, SIGNAL(explosionAt(QPoint)), SLOT(explosionAt(QPoint)));
}

void GameManager::playBombSound()
{
    if (m_soundEnabled) {
        m_explosions[qrand() % 8]->play();
    }
}

void GameManager::explosionAt(const QPoint &position)
{
    for (int i=0; i<m_players.count(); i++) {
        if (m_players[i]->position() == position) {
            if (m_soundEnabled) {
                m_death.play();
            }
            m_players[i]->setAlive(false);
        }
    }
}

void GameManager::endRound()
{
    m_tickTimer.stop();

    for (int i=0; i<m_missiles.count(); i++) {
        m_missiles[i]->deleteLater();
    }
    m_missiles.clear();
    emit missilesChanged();

    for (int i=0; i<m_players.count(); i++) {
        if (!m_players[i]->networkClient()) continue;
        m_players[i]->networkClient()->sendEndOfRound();
    }

    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setCommand(QString());
        if (m_players[i]->isAlive()) {
            m_players[i]->addWin();
        }
    }

    m_roundsPlayed++;
    emit roundsPlayedChanged();

    if (m_roundsPlayed < MAX_ROUNDS) {
        QTimer::singleShot(5000, this, SLOT(startRound()));
    }
}

void GameManager::startRound()
{
    if (m_players.isEmpty()) {
        return;
    }

    resetPositions();

    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setAlive(true);
        m_players[i]->setEnergy(START_ENERGY);
    }

    loadMap(m_map->name());

    // Do not allow to change name after game has started
    for (int i=0; i<m_players.count(); i++) {
        if (!m_players[i]->networkClient()) {
            continue;
        }

        m_players[i]->networkClient()->disconnect(SIGNAL(nameChanged(QString)));
    }

    m_ticksLeft = m_map->maxTicks();
    m_tickTimer.start();
}

void GameManager::resetScores()
{
    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->resetScore();
    }
}

void GameManager::gameTick()
{
    m_ticksLeft--;

    bool missilesRemoved = false;
    QMutableListIterator<Missile*> missileIterator(m_missiles);
    while (missileIterator.hasNext()) {
        Missile *missile = missileIterator.next();

        missile->doMove();

        if (!missile->isAlive()) {
            emit explosion(missile->position());
            missile->deleteLater();
            missileIterator.remove();
            missilesRemoved = true;
            continue;
        }

        for(int i=0; i<m_players.count(); i++) {
            if (missile->owner() == m_players[i]->id()) {
                continue;
            }

            const qreal dx = m_players[i]->position().x() - missile->position().x();
            const qreal dy = m_players[i]->position().y() - missile->position().y();
            if (hypot(dx, dy) < 0.1) {
                m_players[i]->decreaseEnergy(100);
                m_players[missile->owner()]->increaseEnergy(100);
                emit explosion(missile->position());
                missile->deleteLater();
                missileIterator.remove();
                missilesRemoved = true;
                break;
            }
        }
    }

    if (missilesRemoved) {
        emit missilesChanged();
    }


    // Randomize the order we process players in
    QList<QPointer<Player> > players = m_players;
    for (int index = players.count() - 1; index > 0; --index) {
        qSwap(players[index], players[qrand() % (index + 1)]);
    }

    for (int i=0; i<players.count(); i++) {
        QString command = players[i]->command();
        Player *player = m_players[i];
        player->doMove();
        player->decreaseEnergy(1);
        if (command.isEmpty()) {
            continue;
        }

        if (command == "ACCELERATE") {
            player->accelerate();
        } else if (command == "LEFT") {
            player->rotate(-ROTATE_AMOUNT);
        } else if (command == "RIGHT") {
            player->rotate(ROTATE_AMOUNT);
        } else if (command == "BOMB") {
            player->decreaseEnergy(20);
            Missile *missile = new Missile(player->position(), player->rotation(), player->id(), this);
            m_missiles.append(missile);
            emit missilesChanged();
        } else {
            continue;
        }
    }

    int dead = 0;
    for(int i=0; i<players.count(); i++) {
        if (!players[i]->isAlive()) {
            dead++;
        }
    }

    if (dead > 0 && players.size() - dead < 2) {
        emit roundOver();
    } else {
        QList<Player*> players;
        for (int i=0; i<m_players.count(); i++) {
            if (m_players[i]->isAlive()) {
                players.append(m_players[i]);
            }
        }
        for (int i=0; i<players.length(); i++) {
            if (!players[i]->networkClient()) continue;

            QList<Player*> list = players;
            list.takeAt(i);
            players[i]->networkClient()->sendState(list, m_map, players[i]);
        }
    }
}

void GameManager::clientConnect()
{
    QTcpSocket *socket = m_server.nextPendingConnection();

    if (m_players.count() >= m_map->startingPositions().count() || m_tickTimer.isActive()) {
        socket->disconnect();
        socket->deleteLater();
        return;
    }

    addPlayer(new NetworkClient(socket));
}

void GameManager::clientDisconnected()
{
    if (m_tickTimer.isActive()) {
        return;
    }

    // FIXME fix this shit
    Player *playerObject = qobject_cast<Player*>(sender());
    if (!playerObject) {
        qWarning() << "GameManager: invalid sender for disconnect signal";
        return;
    }
    int index = m_players.indexOf(playerObject);
    if (index < 0) {
        qWarning() << "GameManager: unable to find disconnecting client.";
        return;
    }

    m_players.takeAt(index)->deleteLater();
    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setId(i);
    }

    emit playersChanged();
}

void GameManager::addPlayer(NetworkClient *client)
{
    if (m_players.count() >= MAX_PLAYERS) {
        return;
    }

    Player *player = new Player(this, m_players.count(), client);

    m_players.append(player);

    resetPositions();

    if (!client) {
        connect(m_view->rootObject(), SIGNAL(userMove(QString)), player, SLOT(setCommand(QString)));
        player->setName("Local user");
    } else {
        player->setName(client->remoteName());
        connect(player, SIGNAL(clientDisconnected()), SLOT(clientDisconnected()));
    }

    emit playersChanged();
}
void GameManager::kick(int index)
{
    if (m_players.at(index)->networkClient()) {
        m_players.at(index)->networkClient()->kick();
    } else {
        m_players.takeAt(index)->deleteLater();
    }
    emit playersChanged();
}

void GameManager::removeHumanPlayers()
{
    for (int i=0; i<m_players.count(); i++) {
        if (!m_players[i]->networkClient()) {
            m_players.takeAt(i)->deleteLater();
            break;
        }
    }
    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setId(i);
    }

    emit playersChanged();
}

void GameManager::setDebugMode(bool debug)
{
    m_tickTimer.setInterval(debug ? TICKLENGTH_DEBUG : TICKLENGTH);
}

void GameManager::togglePause()
{
    if (m_tickTimer.isActive()) {
        m_tickTimer.stop();
    } else {
        m_tickTimer.start();
    }
}

void GameManager::stopGame()
{
    m_tickTimer.stop();

    foreach(QPointer<Player> player, m_players) {
        if (player->isDisconnected()) {
            player->deleteLater();
        }
    }

    emit playersChanged();
}

void GameManager::resetPositions()
{
    int playerCount = m_players.count();
    for (int i=0; i<playerCount; i++) {
        qreal angle = i * M_PI * 2.0 / playerCount;
        m_players[i]->setPosition(QPointF(cos(angle) * 0.5, sin(angle) * 0.5));
    }
}
