Ghostly
============

You kek and eat other ghosts and pellets and shit.

Graphics are from http://pacman.shaunew.com/redmine/projects/pacman and licensed under the GPLv3

## Windows binary

There are pre-compiled Windows binaries attached to the releases here: https://github.com/sandsmark/aicompo-tg16/releases

There are also automatically built and untested pre-release builds available here: https://iskrembilen.com/turnonme-win32/

---

## Keymap

 * F5: Restarts round
 * p: Pause
 * ESC: Stop game

### Keys for human player

 * Left: turn left
 * Right: turn right
 * Forward: Accelerate
 * m: Fire normal missile.
 * ,: Drop mine.
 * .: Fire homing missile.

---


## Communication Protocol

The protocol is based on JSON. All "packets" (JSON objects) are sent in compact form, terminated with a newline. So to parse, read a line and unserialize the JSON.

To connect to the game, open a TCP connection to where the game is running on port `54321`. When running on the same machine, just connect to localhost (127.0.0.1).

After connecting, send "NAME " followed by the name of your bot to set a name (for example: "NAME superbot\n").

When the game starts, you will start to get "stateupdate" JSON objects, that look like this in expanded form:

```JSON
{
    "messagetype": "stateupdate",
    "gamestate": {
        "missiles": [
            {
                "id": 0,
                "energy": 0,
                "owner": 0,
                "rotation": 180,
                "type": "NORMAL",
                "velocityX": 0.002812616921765813,
                "velocityY": 0.048244483355828163,
                "x": -0.1065170720879374,
                "y": 0.057297497784823674
            },
            {
                "id": 50,
                "energy": 4981,
                "owner": 0,
                "rotation": 45,
                "type": "MINE",
                "velocityX": 0.00042820233654171418,
                "velocityY": -0.0015615237322931463,
                "x": -0.22436009460522804,
                "y": 0.81817305140153029
            },
            {
                "id": 60,
                "energy": 350,
                "owner": 0,
                "rotation": 300,
                "type": "SEEKING",
                "velocityX": 0.032627412854647569,
                "velocityY": -0.03970474002645958,
                "x": -0.086567348713245326,
                "y": 0.69192650021900126
            }
        ],
        "others": [
            {
                "energy": 858,
                "id": 0,
                "rotation": 90,
                "velocityX": -0.007226001197449654,
                "velocityY": -0.026401470594673109,
                "x": -0.42907664254625305,
                "y": 0.40928029816990924
            }
        ],
        "you": {
            "energy": 918,
            "id": 1,
            "rotation": 270,
            "velocityX": 0.0076515503238599285,
            "velocityY": 0.025595966187665863,
            "x": 0.43292444925334561,
            "y": -0.41881639474014176
        }
    }
}
```

There are also two other kinds of messages:

--

`dead` - You've been killed, and cannot expect more data until next round

```JSON
{ "messagetype": "dead" }
```   

--

`endofround` - There's only one player left (for your sake I hope it's you)

```JSON
{ "messagetype": "endofround" }
```

### Controlling

To do something, send the appropriate command followed by `\n`, for example `RIGHT\n`.

The available commands are:

 * `ACCELERATE`: Accelerate in the direction you're currently pointing.
 * `LEFT`: Turn left.
 * `RIGHT`: Turn right.
 * `MISSILE`: Fire a normal missile in the direction you're currently pointing.
 * `SEEKING`: Fire a homing missile that tries to home in on the closest player.
 * `MINE`: Drop a "mine" that tries to hover around the sun.

---

## How to compile

### Manual, on Linux

Check out the code, install the development packages for qt5 declarative and qt5 graphicaleffects, and run: `qmake && make`.

### Alternative

For Windows, OS X, etc.

 * Download and install the Qt SDK from https://www.qt.io/download-open-source/
 * Start QtQtcreator after installing
 * Go to File -> New File or Project -> Import Project -> Git Clone
 * Use the repository URL https://github.com/sandsmark/aicompo-tg16.git
 * Click the green play button.

