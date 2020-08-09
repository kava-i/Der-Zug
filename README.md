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

