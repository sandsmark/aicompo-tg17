var net = require('net');
var HOST = '127.0.0.1';
var PORT = 54321;
var client = new net.Socket();

function onMessage(jsonMessage) {
    if (jsonMessage.messagetype == "stateupdate") {
        // Define available
        var moves = [
            "LEFT\n",
            "RIGHT\n",
            "ACCELERATE\n",
            "MISSILE\n",
            "MINE\n",
            "SEEKING\n"
        ];

        var move = moves[Math.floor(Math.random() * 5)];
        
        // Write our random move to server
        client.write(move);
    }
}

client.connect(PORT, HOST);

client.on('connect', function()
{
    console.log('CONNECTED TO: ' + HOST + ':' + PORT);
    
    // Tell server our name
    client.write('NAME random_nodejs\n');
})

client.on('error', function(data) {
    console.log('ERROR: ' + data);
    client.destroy();
});

var buffer = new Buffer(0);
client.on('data', function(data) {
    var latestBuf = Buffer.concat([buffer, data]);
    buffer = parseBuffer(latestBuf, onMessage);
});

function parseBuffer(buffer, msgCallback) {
    var start = 0;

    while(start < buffer.length) {
        var endOfMsg = buffer.indexOf('\n', start) + 1;
        if(endOfMsg <= 0)
            break;

        msgCallback(JSON.parse(buffer.toString('utf-8', start, endOfMsg)));

        start = endOfMsg;
    }

    return buffer.slice(start, buffer.length);
}

client.on('close', function(error) {
    client.destroy();
    if (error == true)
    {
        console.log("An error occured. Unexpected disconnect");
    } else {
        console.log("Disconnected.");
    }
});

