# Logic

Since entering logic expressions can become quit suffisticated, we could not
completely "automate" this process with dropdowns and buttons, thus some work is
left up to you. Maybe these examples help you to find, what you need. 

The following variables are always accesible: 
- `room` (the id of the current room) 
- `last_room` (the id of the current room) 
- `inventory` (list of item-ids currently in the players inventory
- all attributes 
- all minds 
- in dialogs: `vistited`: all vistited dialog steps 

The following operators exist: `<`, `>`, `=`, `~` (fuzzy match), `:` (in list).
*HINWEIS*: the `:` operator works like this: `inventory:test.potion` 

Assuming the following player-status: 
```
{
    "lp": "0",
    "ep": "25",
    "level": "-1",
    "room": "test.room1",
    "last_room": "test.room2",
    "inventory": "test.trank;test.room2.test.schwert"
}
```

This are possible examples: 

- `"lp=0"` == `true`
- `"ep<30"` == `true`
- `"ep>20"` == `true`
- `"level=-1"` == `true`
- `"room=test.room1"` == `true`
- `"last_room~test.roum2"` == `true`
- `"inventory:test.trank"` == `true`
- `"inventory:test.trank2"` == `false`
- `"inventory:test.room2.test.schwert"` == `true`
- `"!inventory:test.room2.test.waffe"` == `true`
- `"!lp=1"` == `true`
- `"!ep<20"` == `true`
- `"!room=test.room2"` == `true`
- `"!last_room~room2.test"` == `true`
- `"lp=0|1"` == `true`
- `"lp=1|0"` == `true`
- `"lp=1|-1"` == `false`
- `"inventory:test.room2.test.schwert|test.schwert"` == `true`
- `"(lp=0)&&(ep<30)"` == `true`
- `"(lp=0)&&(ep<30)"` == `true`
- `"(lp=0)&&(inventory:test.room2.test.schwert)"` == `true`
- `"(inventory:test.room2.test.schwert)&&(ep<30)"` == `true`
- `"(lp=1)&&(inventory:test.room2.test.schwert)"` == `false`
- `"(lp=0)&&(inventory:test.room2.test.waffe)"` == `false`
- `"(lp=0)||(level=0)"` == `true`
- `"(level=0)||(lp=0)"` == `true`
- `"(lp>-1)||(level=<0)"` == `true`
- `"(last_room~room2.test)||(inventory:test.trank)"` == `true`
- `"(inventory:test.room2.test.waffe)||(last_room~test.roum2)"` == `true`
- `"(lp=0)&&((ep<20)||(ep>24))"` == `true`
- `"(lp=0)&&((ep<20)||(ep>25))"` == `false`
