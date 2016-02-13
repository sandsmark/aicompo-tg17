#include "gamemanager.h"

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
#include <QSettings>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>

#include <cmath>

#define VOLUME 0.5f

GameManager::GameManager(QQuickView *view) : QObject(view),
    m_view(view),
    m_roundsPlayed(0),
    m_ticksLeft(-1),
    m_isGameRunning(false)
{

    // Add QML objects
    m_view->rootContext()->setContextProperty("GameManager", QVariant::fromValue(this));

    // Set up gametick timer
    m_tickTimer.setInterval(DEFAULT_TICKINTERVAL);
    m_tickTimer.setSingleShot(false);

    m_server.listen(QHostAddress::Any, 54321);

    connect(&m_tickTimer, SIGNAL(timeout()), SLOT(gameTick()));
    connect(&m_tickTimer, SIGNAL(timeout()), SIGNAL(tick()));
    connect(&m_server, SIGNAL(newConnection()), SLOT(clientConnect()));
    connect(this, SIGNAL(roundOver()), SLOT(endRound()));
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

QList<QObject*> GameManager::players() const
{
    QList<QObject*> playerList;
    foreach(Player *playerObject, m_players) {
        playerList.append(playerObject);
    }

    return playerList;
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

void GameManager::endRound()
{
    m_tickTimer.stop();

    for (int i=0; i<m_missiles.count(); i++) {
        m_missiles[i]->deleteLater();
    }
    m_missiles.clear();

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

    m_isGameRunning = true;
    emit isGameRunningChanged();

    resetPositions();

    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->setAlive(true);
        m_players[i]->setEnergy(START_ENERGY);
    }

    // Do not allow to change name after game has started
    for (int i=0; i<m_players.count(); i++) {
        if (!m_players[i]->networkClient()) {
            continue;
        }

        m_players[i]->networkClient()->disconnect(SIGNAL(nameChanged(QString)));
    }

    m_tickTimer.start();
}

void GameManager::resetScores()
{
    for (int i=0; i<m_players.count(); i++) {
        m_players[i]->resetScore();
    }
    m_roundsPlayed = 0;
    emit roundsPlayedChanged();
}

void GameManager::gameTick()
{
    m_ticksLeft--;

    QMutableListIterator<Missile*> missileIterator(m_missiles);
    while (missileIterator.hasNext()) {
        Missile *missile = missileIterator.next();

        missile->doMove();

        if (!missile->isAlive()) {
            missile->deleteLater();
            missileIterator.remove();
            continue;
        }

        Player *closest = nullptr;
        qreal closestDX;
        qreal closestDY;
        for(int i=0; i<m_players.count(); i++) {
            if (!m_players[i]->isAlive()) {
                continue;
            }

            if (missile->owner() == m_players[i]->id()) {
                continue;
            }

            const qreal dx = m_players[i]->position().x() - missile->position().x();
            const qreal dy = m_players[i]->position().y() - missile->position().y();
            if (hypot(dx, dy) < 0.1) {
                m_players[i]->decreaseEnergy(MISSILE_DAMAGE);
                m_players[missile->owner()]->increaseEnergy(MISSILE_DAMAGE);
                m_players[missile->owner()]->addScore(1);
                emit explosion(missile->position());
                missile->deleteLater();
                missileIterator.remove();
                break;
            }

            // For seeking missiles
            if (!closest || hypot(dx, dy) < hypot(closestDX, closestDY)) {
                closest = m_players[i];
                closestDX = dx;
                closestDY = dy;
                continue;
            }
        }

        if (missile->type() == Missile::Seeking && closest) {
            missile->setRotation(atan2(closestDY, closestDX));
        }
    }


    // Randomize the order we process players in
    QList<Player*> players = m_players;
    for (int index = players.count() - 1; index > 0; --index) {
        qSwap(players[index], players[qrand() % (index + 1)]);
    }

    int dead = 0;
    foreach(Player *player, players) {
        if (!player->isAlive()) {
            dead++;
            continue;
        }

        player->doMove();
        player->decreaseEnergy(1);

        QString command = player->command();
        if (command.isEmpty()) {
            continue;
        }

        if (command == "ACCELERATE") {
            player->accelerate();
        } else if (command == "LEFT") {
            player->rotate(-ROTATE_AMOUNT);
        } else if (command == "RIGHT") {
            player->rotate(ROTATE_AMOUNT);
        } else if (command == "MISSILE") {
            player->decreaseEnergy(MISSILE_COST);
            Missile *missile = new Missile(Missile::Normal, player->position(), player->rotation(), player->id(), this);
            m_missiles.append(missile);
            emit missileCreated(missile);
        } else if (command == "SEEKING") {
            player->decreaseEnergy(SEEKING_MISSILE_COST);
            Missile *missile = new Missile(Missile::Seeking, player->position(), player->rotation(), player->id(), this);
            m_missiles.append(missile);
            emit missileCreated(missile);
        } else if (command == "MINE") {
            player->decreaseEnergy(MINE_COST);
            Missile *missile = new Missile(Missile::Mine, player->position(), player->rotation(), player->id(), this);
            m_missiles.append(missile);
            emit missileCreated(missile);
        }
    }

    if (dead > 0 && players.size() - dead < 2) {
        emit roundOver();
        return;
    }

    // Send status updates to all connected players
    foreach(Player *player, players) {
        if (!player->networkClient()) {
            continue;
        }

        player->networkClient()->sendState(serializeForPlayer(player));
    }
}

void GameManager::clientConnect()
{
    QTcpSocket *socket = m_server.nextPendingConnection();

    if (m_players.count() >= MAX_PLAYERS || m_tickTimer.isActive()) {
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

void GameManager::setTickInterval(int interval)
{
    m_tickTimer.setInterval(interval);
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
    m_isGameRunning = false;
    emit isGameRunningChanged();

    QMutableListIterator<Player*> it(m_players);
    while(it.hasNext()) {
        Player *player = it.next();
        if (player->isDisconnected()) {
            player->deleteLater();
            it.remove();
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

QJsonObject GameManager::serializeForPlayer(Player *player)
{
    QJsonObject gamestateObject;
    gamestateObject["you"] = player->serialize();

    QJsonArray playersArray;
    foreach (Player *otherPlayer, m_players) {
        if (otherPlayer == player) {
            continue;
        }
        if (!otherPlayer->isAlive()) {
            continue;
        }

        playersArray.append(otherPlayer->serialize());
    }
    gamestateObject["others"] = playersArray;

    QJsonArray missilesArray;
    foreach (Missile *missile, m_missiles) {
        missilesArray.append(missile->serialize());
    }
    gamestateObject["missiles"] = missilesArray;

    return gamestateObject;
}

//void GameManager::createMissileSprite(Missile *missileObject)
//{
//    static QQmlComponent *spriteComponent = nullptr;

//    if (!spriteComponent) {
//        spriteComponent = new QQmlComponent(m_view->engine(), QUrl("qrc:/qml/MissileSprite.qml"), QQmlComponent::PreferSynchronous);
//    }
//    Q_ASSERT(spriteComponent);


//    if (spriteComponent->status() != QQmlComponent::Ready) {
//        qWarning() << Q_FUNC_INFO << "Component not ready:" << spriteComponent->errorString();;
//        return;
//    }

//    QQuickItem *gameArea = qobject_cast<QQuickItem*>(m_view->rootObject()->findChild<QObject*>("gameArea"));
//    if (!gameArea) {
//        qWarning() << Q_FUNC_INFO << "Unable to find game area";
//        return;
//    }


//    QObject *missileSpriteObject = spriteComponent->create();
//    if (!missileSpriteObject) {
//        qWarning() << Q_FUNC_INFO << "Unable to create missile sprite";
//        return;
//    }
//    QQuickItem *missileSprite = qobject_cast<QQuickItem*>(missileSpriteObject);
//    if (!missileSprite) {
//        qWarning() << Q_FUNC_INFO << "Invalid missile sprite";
//        missileSpriteObject->deleteLater();
//        return;
//    }

//    missileSprite->setParent(missileObject);
//    missileSprite->setParentItem(gameArea);
//    missileSprite->setProperty("missileData", QVariant::fromValue(missileObject));
//}
