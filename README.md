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
