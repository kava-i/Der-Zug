# Listeners/Events

## Objekte (Räumen) hinzufügen, entfernen, oder bewegen
Alle diese Funktionen sind generell sehr vielfältig einsetzbar und können für
sämtliche Gegenstände (Items), Details und Personen (Characters) eingesetzt
werden.
Allerdings kann es sein, dass diese Funktionen in zwei Fällen nicht
funktionieren: 
- Wenn du z.B. mehrere gleiche Objekte in einem Raum hast, kann zwar das *Spiel*
  diese Objekte für den normalen Spielfluss unterscheiden, indem es die IDs
  anpasst, aber das ist *vor* dem Spiel noch nicht richtig abzusehen. 
  Deshalb solltest du diese Funktionen nur verwenden für *besondere* Gegenstände (z.B.
  Quest-Gegenstände, Hauptpersonen oder das Verschwinden sehr wichtiger Raum-Details 
  ("huch, da war doch gerade noch ein Spiegel..."). 
- Es kann auch dazu kommen, dass mehrere Objekte in einem Raum sind, weil du mit
  `addItemToRoom` einen Gegenstand in den Raum hinzugefügt hast, der bereits
  existierte. (Dann wird an die ID des neuen Gegenstands eine `_2` hinzugefügt und er kann
  nicht mehr mit der ursprünglichen ID gefunden werden.) Um das zu vermeiden,
  halte dich an den obigen Vorschlag.

#### Add item|detail|char to room: 
Erstellt ein neues Objekt (Item, Detail, Character) und fügt es dem entsprechenden Raum hinzu. 

*Hinweis*: Fall das Objekt im Raum bereits existiert, wird an die id ein `_2`, `_3`, ...
angehängt. Dadurch kann es sein, dass es später nicht mehr mit `removeXFromRoom`
oder `moveXToRoom` gefunden werden kann!

*Hinweis*: Dialoge sind aktuell noch immer die gleichen. D.h. wenn ein Character
neu erstellt wird, der bereits existierte, ist der dialog *derselbe*, jede
Änderung an dem Dialog des ersten characters wird auch am zweiten sichtbar sein.
Sie "teilen" sich quasi einen Dialog. (Daher ist es empfohlen nie bereits
existierende charactere mit `addCharToRoom` hinzuzufügen, sondern stattdessen
`moveCharToRoom` zu verwenden.

**Listener** 
- h_addItemToRoom(item_id, room_id)
- h_addDetailToRoom(char_id, room_id)
- h_addCharToRoom(detail_id, room_id)

**Events**
- addItemToRoom(item_id, room_id)
- addDetailToRoom(char_id, room_id)
- addCharToRoom(detail_id, room_id)


#### Remove item|detail|char from room: 
Entfernt ein Objekt von dem entsprechenden raum und löcht es. 
*Hinweis*: falls du das Objekt *nicht* löschen magst, erstelle einen Raum, der
im Spiel nie bereteten wir (ich verwendee gerne `non-exist`) und verwende statt
`removeXFromRoom` den `moveXToRoom`

**Listener** 
- h_removeItemFromRoom(item_id, room_id)
- h_removeDetailFromRoom(char_id, room_id)
- h_removeCharFromRoom(detail_id, room_id)

**Events**
- removeItemFromRoom(item_id, room_id)
- removeDetailFromRoom(char_id, room_id)
- removeCharFromRoom(detail_id, room_id)

#### Move item|detail|char to room:
Erstellt weder ein neues Objekt noch löcht es ein altes, sondern bewegt
einfach ein objekt von einem raum in den nächsten

*Hinweis*: die angegebene id (die noch zum alten Raum gehört) wird NICHT angepasst. 
Das Objekt bleibt daher immer unter der gleichen ID auffindbar.

**Listener** 
- h_moveItemToRoom(item_id, from_room_id, to_room_id)
- h_moveDetailToRoom(char_id, from_room_id, to_room_id)
- h_moveCharToRoom(detail_id, from_room_id, to_room_id)

**Events**
- moveItemToRoom(item_id, from_room_id, to_room_id)
- moveDetailToRoom(char_id, from_room_id, to_room_id)
- moveCharToRoom(detail_id, from_room_id, to_room_id)
