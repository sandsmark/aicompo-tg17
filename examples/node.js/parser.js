/**
 * Parses the map to a weight-based system
 * @param data
 * @returns {Array}
 * @constructor
 */
exports.ParseMap = function (data) {
    var map = [];

    for (var i = 0; i < data.length; i++) {
        var t = [];
        for (var j = 0; j < data[i].length; j++) {
            t.push(Weight(data[i][j]));
        }
        map.push(t);
    }
    //console.log("In ParseMap")
    return map;
};

/**
 * Weights each point according to whatever value we have
 * defined for that point-type.
 * @param point
 * @returns {number}
 * @constructor
 */
function Weight(point) {
    // Tile weighting - higher is more attractive, 0 is no-go
    var weight = {
        floor: 1,
        wall: 0,
        door: 1,
        pellet: 2,
        superPellet: 3
    };

    switch (point)
    {
        case '|':
            return weight.wall;

        case '_':
            return weight.floor;

        case '-':
            return weight.door;

        case '.':
            return weight.pellet;

        case 'o':
            return weight.superPellet;

        case '#':
            return weight.floor;

        case '@':
            return weight.floor;
    }
}

/**
 * Calculates the distance from you to all the players
 * @param players
 * @param you
 * @constructor
 */
exports.CalcDist = function (players, you) {
    var pld = new Array(players.length);

    for (var i = 0; i < players.length; i++) {
        pld[i] = lineDistance(players[i], you);
    }

    Array.min = function (array) {
        return Math.min.apply(Math, array);
    };

    var result = pld;
    pld = null;

    return (Array.min(result));
};

/**
 * Returns the distance from one {x: x, y: y} to another.
 * @param point1
 * @param point2
 * @returns {number}
 */
function lineDistance(point1, point2) {
    var xs = 0;
    var ys = 0;

    xs = point2.x - point1.x;
    xs = xs * xs;

    ys = point2.y - point1.y;
    ys = ys * ys;

    return Math.sqrt(xs + ys);
}