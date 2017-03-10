Ghostly
============

A game similar to Pacman™, but where you are the ghosts.

You create an AI that controls one of the four ghosts. Initially you can't pass through other ghosts, so watch out for getting blocked in. You gain points by eating pellets, and by eating a super pellet you can eat other ghosts. The round is over when either the last pellet is gone or there is only one player left.

Original idea by Kepler.

Graphics are from http://pacman.shaunew.com/redmine/projects/pacman and licensed under the GPLv3.

## Windows binary

There are pre-compiled Windows binaries attached to the releases here: https://github.com/sandsmark/ghostly/releases

There are also automatically built and untested pre-release builds available here: https://iskrembilen.com/ghostly-win32/

---

## Keymap

 * F5: Restarts round
 * p: Pause
 * ESC: Stop game

### Keys for human player

 * Up: Go up.
 * Left: turn left
 * Right: turn right
 * Down: Go Down.

---


## Communication Protocol

The protocol is based on JSON. All "packets" (JSON objects) are sent in compact form, terminated with a newline. So to parse, read a line and unserialize the JSON. All transferred text is expected to be UTF-8.

To connect to the game, open a TCP connection to where the game is running on port `54321`. When running on the same machine, just connect to localhost (127.0.0.1).

After connecting, send "NAME " followed by the name of your bot to set a name (for example: "NAME superbot\n").

--

When you connect you get a `welcome` message that looks like this when expanded:

```JSON
{
    "map": {
        "content": [
            "||||||||||||||||||||||||||||",
            "|............||............|",
            "|.||||.|||||.||.|||||.||||.|",
            "|o||||.|||||.||.|||||.||||o|",
            "|.||||.|||||.||.|||||.||||.|",
            "|..........................|",
            "|.||||.||.||||||||.||.||||.|",
            "|.||||.||.||||||||.||.||||.|",
            "|......||....||....||......|",
            "||||||.|||||_||_|||||.||||||",
            "_____|.|||||_||_|||||.|_____",
            "_____|.||__________||.|_____",
            "_____|.||_|||__|||_||.|_____",
            "||||||.||_|______|_||.||||||",
            "______.___|______|___.______",
            "||||||.||_|______|_||.||||||",
            "_____|.||_|||__|||_||.|_____",
            "_____|.||__________||.|_____",
            "_____|.||_||||||||_||.|_____",
            "||||||.||_||||||||_||.||||||",
            "|............||............|",
            "|.||||.|||||.||.|||||.||||.|",
            "|.||||.|||||.||.|||||.||||.|",
            "|o..||.......__.......||..o|",
            "|||.||.||.||||||||.||.||.|||",
            "|||.||.||.||||||||.||.||.|||",
            "|......||....||....||......|",
            "|.||||||||||.||.||||||||||.|",
            "|.||||||||||.||.||||||||||.|",
            "|..........................|",
            "||||||||||||||||||||||||||||"
        ],
        "height": 31,
        "pelletsleft": 240,
        "width": 28
    },
    "messagetype": "welcome",
    "you": {
        "id": 0,
        "x": 11,
        "y": 13
    }
}
```

--
When the game starts, you will start to get "stateupdate" messages after each game tick has been processed, which looks like this expanded:

```JSON
{
    "gamestate": {
        "map": {
            "content": [
                "||||||||||||||||||||||||||||",
                "|............||............|",
                "|.||||.|||||.||.|||||.||||.|",
                "|o||||.|||||.||.|||||.||||o|",
                "|.||||.|||||.||.|||||.||||.|",
                "|..........................|",
                "|.||||.||.||||||||.||.||||.|",
                "|.||||.||.||||||||.||.||||.|",
                "|......||....||....||......|",
                "||||||.|||||_||_|||||.||||||",
                "_____|.|||||_||_|||||.|_____",
                "_____|.||__________||.|_____",
                "_____|.||_|||__|||_||.|_____",
                "||||||.||_|______|_||.||||||",
                "______.___|______|___.______",
                "||||||.||_|______|_||.||||||",
                "_____|.||_|||__|||_||.|_____",
                "_____|.||__________||.|_____",
                "_____|.||_||||||||_||.|_____",
                "||||||.||_||||||||_||.||||||",
                "|............||............|",
                "|.||||.|||||.||.|||||.||||.|",
                "|.||||.|||||.||.|||||.||||.|",
                "|o..||.......__.......||..o|",
                "|||.||.||.||||||||.||.||.|||",
                "|||.||.||.||||||||.||.||.|||",
                "|......||....||....||......|",
                "|.||||||||||.||.||||||||||.|",
                "|.||||||||||.||.||||||||||.|",
                "|..........................|",
                "||||||||||||||||||||||||||||"
            ],
            "height": 31,
            "pelletsleft": 240,
            "width": 28
        },
        "others": [
        ],
        "you": {
            "id": 0,
            "x": 11,
            "y": 13,
            "score": 130,
            "isdangerous": true
        }
    },
    "messagetype": "stateupdate"
}
```

There are also two other kinds of messages:

--

`dead` - You've been killed, and cannot expect more data until next round

```JSON
{ "messagetype": "dead" }
```

--

`endofround` - There's only one living player left. If you received the above message right before this one, that ain't you.

```JSON
{ "messagetype": "endofround" }
```

### Controlling

To do something, send the appropriate command followed by `\n`, for example `RIGHT\n`. The commands are case insensitive.

The available commands are:

 * `UP`: Move up.
 * `DOWN`: Move down.
 * `LEFT`: Move left.
 * `RIGHT`: Move right.

### Map format

The different kinds of tiles in the map are:

 * `_`: A normal floor tile.
 * `|`: A wall tile.
 * `-`: A door tile.
 * `.`: A floor tile with a pellet on it.
 * `o`: A floor tile with a superpellet on it.

The follow tiles only appear in the map file, not in the JSON messages:

 * `#`: A floor tile with a starting position for a player.
 * `@`: A floor tile with the starting position for the sudden death monster.


## Command line arguments

It supports several command line arguments for more advanced usage:

```
Usage: ./ghostly [options]

Options:
  -h, --help                Displays this help.
  --start-at <players>      Automatically start the game after <players> (1 -4)
                            have connected, default 4
  --tick-interval, -i <ms>  Set the tick interval to <ms> milliseconds (10 -
                            1000), default 100
  --quit-on-finish          Exit the game after playing all rounds
  --fullscreen              Start the game in fullscreen
  --rounds <rounds>         Set the number of rounds to <rounds>, default 4.
                            Set to -1 to play forever.
  --headless                Run without showing the UI, e. g. for running on a
                            server. This implies the start-at and quit-on-finish
                            options.
  --tickless                Run without a timer, execute a game tick as soon as
                            it has received commands from all players
```

It also prints debug output to stderr, and informative messages to stdout, for automatic parsing.


## How to compile

### Manual, on Linux

Check out the code, install the development packages for qt5 declarative and qt5 graphicaleffects, and run: `qmake && make`.

### Alternative

For Windows, OS X, etc.

 * Download and install the Qt SDK from https://www.qt.io/download-open-source/
 * Start QtQtcreator after installing
 * Go to File -> New File or Project -> Import Project -> Git Clone
 * Use the repository URL https://github.com/sandsmark/ghostly.git
 * Click the green play button.



*PAC-MAN™ is the registered trademark of BANDAI NAMCO Entertainment, which is not affiliated with nor endorses this software.*
