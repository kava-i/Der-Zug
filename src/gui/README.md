# GameCreator

## Objects

## Characters

#### Name
The name of a character need not by unique.

#### Id
The id must be unique within one area

#### Music
The id of any sound file added at `sounds`.

This sound will then be played, when a dialog with this character starts.

#### Image
The id of any image file added at `images`.

The background-image will changed to this image, when a dialog with this character starts.

#### Description
For more details, see [Description](#description)

Description showm when triggering `examine [name]`.

#### Room Description
For more details, see [Description](#description)

Description automatically added to the description of a room.

#### Dead Description
For more details, see [Description](#description)

Description showm when triggering `examine [name]` when character is dead.

#### Default Description
To use default descriptions, create a category in `texts` name f.e. `default_people` 
(you can use any name).  In this category, you may create any number of texts/ descriptions 
and you may use any name for these texts.

Then you can set `Default Description` to `default_people`. When loading the
word this character will be asign a random text/ description from the category
`default_people` in `texts`.

#### Items
List of items. First element is the id of an item, used as "template", the
second is a json, in whcih every attribute of [item](#item) can be overwritten.

#### Attacks
List of attacks. You need to fill in the extact id of the attack.

#### Handlers

- `function`: the function to call, when event is triggered.
- `info`: a json with information (which infos an event needs is listed at [events](events))
- `cmd (string)`: use *either* this, or `cmd (regex) **!**. The command to listen to (exact-match).
- `cmd (regex)`: use *either* this, or `cmd (string) **!**. The command to listen to (regex-match).

#### Dialog
The dialog a charact must use have the following format: `[character_id]_[number]`.
These means, you can potentially create any number of different dialogs for a
character, the one the character initialy has, is the dialog, which id end with
`_1`. During the game you can that change the dialog of a character.

Alternativly, you can specify a [defaultDialog](#default_dialog), selecting a
random default dialog of a specified category. If no default-dialog-type is specified
and no character-dialog (dialog with id `[character_id]_[number]`) exists,
GameCreator will try to set the dialog `defaultDialog`, which should exist,
already, when creating your world.

#### Default Dialog<a name="default_dialog"></a>

Assume, you created (the apart from a few others) these dialogs:
- default_travaller_1
- default_travaller_2
- default_travaller_3
- default_travaller_4

If now you set `defaultDialog` to `default_travaller`, the character will end up
using one of default_travaller_1-4. Of Caurse you can create default-dialogs with any 
name, however we suggest using `default_[type-of-characters]_[number]` as a
convention.


#### "Classic" attributes
- `HP`: Max hp. Default: `10`.
- `Strength`: strength (added to damage, when fighting). Default: `10`.
- `EP`: experience gain, when defeating character in battle. Default: `10`.
- `Faint`: when set to `yes`, character "only" faints (instead of dying), after being defeating. Default: `yes`


## Items<a name="item"></a>


## Concepts 

### Description<a name="description"></a>


### Events <a name="events"></a>
If not further specialized: 
- `room_id=area.room_id`
- `char_id=room_id.char_id`
- `detail_id=room_id.detail_id`

- `addCharToRoom <char_id|room_id>` 
- `removeCharFromRoom <char_id>` (room-id is implizitly taken from char-id)
- `addDetailToRoom <detail_id|room_id>`
- `addDetailToRoom <detail_id|room_id> (here detail_id is the *simple* id, without
  room id)
- `removeHandlerFromRoom <room_id|handler_id>` (handler_id is simple)
