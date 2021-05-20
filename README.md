# Der-Zug
Der Zug, Textadventure for bebarengan sepur

__Quests__
Quests are build up as a list of steps.
All steps may have a listener, triggering "quest-solved" and certain actions, when specific events take place. (f.e. a listener might be reacting to "pick up Apfel von Kulikunde", so when this is entered, the listener triggers) 
The listeners may either be custom functions (which should be named as following: "h\_[stepID]", or a standard listener "h_react". This standard listener reacts to the set string attribute. f.e. {"id": "h_react, "string":"go"} reacts on every "go to" command. You may then add a map of _infos_ to the quest-step, to specify the current room of the player ("location"), a item in inventory ("inventory"), or a (fuzzy-)-match to the players input ("string"/"fuzzy"). F.e. if ("string":"corridor"), h_react, triggers when entering "go to compartment", but the fails, caus "compartment" does not match" corridor".

There are two kind of quests:
1. online from the beginning
2. online, when set active.

Both kind of quests have a few implications:
_1. Online from Beginning_
A context for the quest is added to the context-stack. Furthermore, all listeners are set at the beginning of the game. So every quest will be marked as solved, as soon as the listener triggers accordingly.

_2. Online when set active_
A context for the quest is only add to context-stack, when quest is set active, furthermore, all listeners are only added to the context's event-manager, when the step holding the listener, is set active.


## Concerning IDs
How do we assure that
1. ...no duplicated IDs are used or accidentally created.
2. ...when creating a new object (room, character, what ever) you don't need to
   write or memorize long ids.

__Why is this neccecary?__
Obviously, becouse one might want to create mulpiple characters with the same
name, or multible real instances of the same kind (f.e. 5 Zombies) and at the
same time being able to adress them with out using randomly generated ids.

__Solution__: 
Multiple characters with the same name simply need different ids. 
Concerning f.e. two characters called _Jane_ with one being the friend of _Mary_
the other the daugther of _Tobi_, call Jane1 _jane-marys-friend_ the othe
_jane-tobis-daugther_

Apart from that, all characters (or rooms, items, etc.) referenced in a context,
will get the context added to the id. 
Consider f.e. the item _waterbottle_ in a cupboard. The id of "waterbottle" will
be changed to "<cupboard-id>.waterbottle". The cupboard-id will also depend on
the context.
This will be added when starting the game itself. So, when adding "waterbottle" 
to the cupboard, one only needs to type: "waterbottle".

Another example occurs, when dealing with rooms and exits. 
All rooms are in so-called _areas_. Concider the following room structure:
- toms-flat
  - corridor
  - bathroom
  - living-room
  - bedroom
- janes-flat
  - corridor
  - bathroom
  - living-room
  - bedroom

No if I want to add an exit from janes living-room to janes-bathroom I only need
to enter "bathroom" as an exit, which will internally be changed to:
"janes-flat.bathroom"
What though if I want to move from janes corridor to toms corridor (assuming
this would be possible). 
_If you want to change the context, the full path needs to be supplied. When
creating the word, full-paths are not changed._ 
