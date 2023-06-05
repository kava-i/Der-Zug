# General 

## Listeners, Handlers, Events

**Listeners "listen" or "wait" for events to happen or be "triggered" and the
execute handlers to "handle" the event in a certain way**

### Events 
Events are things that *happen* an event might be user input: 
```
go to kitchen
```
Or a triggered event in result of other stuff like 
```
setAttribute strength|+|1
```

Typically the first part of an event is a "command", here to *go* to the kitchen
(`go to`) or to set/update an Attribute (`setAttribute`) whereas the second part
is the "target": go to the *kitchen*, increase the attibute *strength by 1*.

### Listeners 
For things to happen when an event is triggered there must be something
"listening" to these events, these are listeners.

There are system listeners 

go to [listeners](/listeners.md) to seem them explained in more detail.

## Logic: 
To see a few tested examples for how to use the `logic` field (texts,
quest-steps), see: [logic](/logic.md) 

## Some notes on general game mechanics

### Accessing nth person, detail, etc. 
If there are f.e. multiple details with the same name in one room, are multible
enemies to aim an attack at in a fight, you may add a number the name to access
the nth person or detail. (This may not work in all cases, but should in the
most. Please report an issue if you believe an important case is not handled)
f.e.: 
```
>zeige details
Hier sind drei Kisten
>shaue in kiste
Hier ist nichts zu finden
>schaue in kiste-2
Hier ist eine Kassettenplayer
```
