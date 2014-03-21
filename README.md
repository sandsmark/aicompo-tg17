Drop da Bomb
============

### Default protocol 

**Don't know this, someone else must fix**

---

### JSON Protocol

On each game tick the server emits one or more JSON-objects, depending on what happened the last tick.

`status update` - Contains all essential information for your player

```JSON
{
    "type": "status update",
    "players": [
        {
            "id": 0,
            "x": 1,
            "y": 1
        }
    ],
    "bombs": [
        {
            "x": 2,
            "y": 1,
            "state": 0
        }
    ],
    "x": 10,
    "y": 1,
    "height": 13,
    "width": 17,
    "map": [
        "++++++++",
        "+..####+",
        "+.#####+",
        "+######+",
        "+######+",
        "+#####.+",
        "+####..+",
        "++++++++"
    ]
}
```

--

`dead` - You've been killed, and cannot expect more data until next round

```JSON
{ "type": "dead" }
```   

--

`end round` - There's only one player left (for your sake I hope it's you)

```JSON
{ "type": "end round" }
```

---

### The map

This example map can you add into the maps-folder in your game-folder. It'll be parsed automatically and you can use it to play on. Be creative, and test as many map combinations as possible to be sure your AI can cover all kinds of resistance! :)

```
8x8     Mapsize in XxY
2       Number of spawns
1,1     Spawn coordinates in X,Y
6,6     Spawn coordinates in X,Y
++++++++
+_.####+
+.#####+
+######+
+######+
+#####.+
+####._+
++++++++

+     Walls that can not be destroyed
#     Walls that can be destroyed
.     Grass
_     Spawn(s)
```

When you are playing, the map looks a bit different. All walkable tiles have been converted to a dot (`.`), so the map above would now look like the one below, really not much of a difference.

```
++++++++
+..####+
+.#####+
+######+
+######+
+#####.+
+####..+
++++++++
```

---

### The bombs

Watch out! They sting!

On a serious note though, not too much about them except for the `state` property. This tells you how many ticks the bomb has left to live. At a state of 0, the bomb will explode in a horizontal and vertical direction, covering the total of 5 tiles vertically and horizontally creating a + sign.

---

### Playing the game!

When first connecting to the server, you have the possibility to emit the message `JSON\n`. This will make the server emit all the data in JSON-format.   
Next is setting your name, this is done by emitting `NAME RandomBot\n` to the server.

When you have joine a game you have the possibility to interact in 5 ways; navigating in 4 directions, and **dropping da bomb**!

```
"LEFT\n"
"RIGHT\n"
"UP\n"
"DOWN\n"
"BOMB"\n"
```

Easy, right? :)

Last but not least, the bots can throw shit at each other! For instance `SAY Puny humans!\n`

**Remember:** Always end a command with `\n`
