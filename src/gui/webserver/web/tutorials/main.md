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
