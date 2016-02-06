var net = require('net');
var HOST = '127.0.0.1';
var PORT = 54321;
var client = new net.Socket();

// Connect to server
client.connect(PORT, HOST);

// Emitted when connection is succesful
client.on('connect', function()
{
    console.log('CONNECTED TO: ' + HOST + ':' + PORT);
    
    // Tell server that we want JSON
    client.write('JSON\n');
    
    // Tell server our name
    client.write('NAME random_nodejs\n');
})

// Triggered when an error occurs in the connection
client.on('error', function(data) {
    console.log('ERROR: ' + data);
    client.destroy();
});

// Triggered each time the server sends us something
client.on('data', function(data) {

    // Splits data on \n
    var d = data.toString("utf-8").split("\n");

    // Removes last object in array, as it always is a \n
    d.pop();

    // Loop through datastrings
    for (var i = 0; i < d.length; i++)
    {
        var obj = JSON.parse(d[i]);
        if (obj.type == "status update") {
            // Define available
            var moves = [
                "UP\n",
                "LEFT\n",
                "RIGHT\n",
                "ACCELERATE\n",
                "MISSILE\n",
                "MINE\n",
                "SEEKING\n"
            ];
            
            // Write our random move to server
            client.write(moves[Math.floor(Math.random() * 5)]);
        }
    }
});

// Triggered when connection closes
client.on('close', function(error) {
    client.destroy();
    if (error == true)
    {
        console.log("An error occured. Unexpected disconnect");
    } else {
        console.log("Disconnected.");
    }
});


