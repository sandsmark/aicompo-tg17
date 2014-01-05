#ifndef USERPLAYER_H
#define USERPLAYER_H

#include "player.h"

class UserPlayer : public Player
{
    Q_OBJECT

public:
    explicit UserPlayer(QObject *parent, int id);


private:

};

#endif // USERPLAYER_H
