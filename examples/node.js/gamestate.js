var parser = require('./parser.js');
var Navigator = require('./navigation.js');

/**
 * Gamestate constructor
 * @param socket
 */
function gameState(socket)
{
    // Initial variables
    this.socket = socket;
    this.state = {};
    this.map = [];
    this.me = {
        id: 0,
        x: 0,
        y: 0,
        score: 0,
        isDangerous: true
    };
    this.pellets = 0;
}

/**
 * Receive data from server, and start doing stuff
 * @param data
 */
gameState.prototype.update = function(data)
{
    switch (data.messagetype)
    {
        case 'welcome':
            this.state = data;
            this.map = parser.ParseMap(this.state.map.content);
            this.me.id = this.state.you.id;
            this.me.x = this.state.you.x;
            this.me.y = this.state.you.y;
            this.pellets = this.state.map.pelletsleft;
            break;

        case 'dead':
            console.log('I ded...');
            break;

        case 'startofround':
            console.log('LET\'S DO THIS!!!!');
            break;

        case 'endofround':
            console.log('Round is over - nothing more to do');
            break;

        case 'stateupdate':
            this.state = data.gamestate;
            this.me.x = this.state.you.x;
            this.me.y = this.state.you.y;
            this.me.score = this.state.you.score;
            this.me.isDangerous = this.state.you.isdangerous;
            this.pellets = this.state.map.pelletsleft;

            this.map = parser.ParseMap(this.state.map.content);
            this.findMove();
            break;
    }
};

/**
 * Find a move and execute it
 */
gameState.prototype.findMove = function() {
    // This is where you would some sort of path finding algorithm

    var navigator = new Navigator(this.map);
    var move = navigator.randomLegalMove(this.me.x, this.me.y);
    // var move = navigator.randomMove();
    this.sendToServer(move);
};

/**
 * Send input to server
 * @param input
 */
gameState.prototype.sendToServer = function(input)
{
    if (typeof input === 'string') {
        this.socket.write(input);
        console.log('Sent command to server: '+ input);
    } else {
        console.log('Not sending command to server: Not a string');
    }
};

module.exports = gameState;