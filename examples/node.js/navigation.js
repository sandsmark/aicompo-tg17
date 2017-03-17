module.exports = navigator;

/**
 * Navigator constructor
 * @param map
 */
function navigator(map)
{
    this.map = map;
    this.moves = [
        'RIGHT\n',
        'LEFT\n',
        'DOWN\n',
        'UP\n'
    ];
}

/**
 * Gives you a random direction to move in
 */
navigator.prototype.randomMove = function()
{
    return this.moves[Math.floor(Math.random()* 4)];
};

/**
 * Check if a move is legal
 * @param move
 * @param point
 * @returns {boolean}
 */
navigator.prototype.isMoveLegal = function(move, x, y) {
    switch (move)
    {
        case this.moves[0]: // Right
            if (this.map[y][x+1] == 0) {
                console.log('Right is not a valid move');
                return false;
            }
            break;

        case this.moves[1]: // Left
            if (this.map[y][x-1] == 0) {
                console.log('Left is not a valid move');
                return false;
            }
            break;

        case this.moves[2]: // Down
            if (this.map[y+1][x] == 0) {
                console.log('Down is not a valid move');
                return false;
            }
            break;

        case this.moves[3]: // Up
            if (this.map[y-1][x] == 0) {
                console.log('Up is not a valid move');
                return false;
            }
            break;
    }

    return true;
};

navigator.prototype.randomLegalMove = function(x, y)
{
    console.log(x +', '+ y);
    var move = this.randomMove();
    if (!this.isMoveLegal(move, x, y)) {
        return this.randomLegalMove(x, y);
    } else {
        return move;
    }
}

/**
 * Gives you the opposite direction of a move. Useful if you have to turn
 * around to where you came from
 */
navigator.prototype.opposite = function(move)
{
    switch(move)
    {
        case this.moves[0]: // Right
            return this.moves[1];
        case this.moves[1]: // Left
            return this.moves[0];
        case this.moves[2]: // Down
            return this.moves[3];
        case this.moves[3]: // Up
            return this.moves[2];
    }
};