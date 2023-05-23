# Config

Hier hast du die Möglichkeit ein Menge an Spiel-Details anzupassen.

## Printing 

`Printing` hat mehrere Funktionen: 

1. In erster Linie dient es dazu spiel-interne Texte mit einem bestimmten
   "Speaker" zu versehen. Die vier Kategorien von spiel-internen Texten sind: 
   - `desc`: alle Beschreibungen von z.B. Ausgänge eines Raumes etc. 
   - `story`: z.B. die Nachricht, wenn das Spiel zu Ende ist, oder wenn du einen
     Chat mit einer anderen Spieler:in beginnst 
   - `error`: Diverse Fehler-Meldungen, die an die Spieler:in weitergeleitet
     werden.
   - `tech`: Diverse technische Meldungen, die an die Spieler:in weitergeleitet
     werden.
2. Diese definierten Sprecherinnen können aber auch verwendet werden, um andere
   Sprecherinnen vorzudefinieren. Falls du z.B. oft Texte von einem Erzähler
   sprechen lässt, kannst du hier das Attribut `story` zu "ERZÄHLER" setzten und
   kannst du an anderer Stelle als `Speaker` ebenfalls "story" schreiben. story
   wird dann automatisch durch "ERZÄHLER" ersetzt. 
   Dies kann 2 Vorteile haben: 
   - Falls du dich entscheidest den Erzähler zu ändern (z.B. hättest du lieber
     eine weibliche Erzähler-Stimme, statt einer männlichen) musst du nur hier
     `story="ERZÄHLER"` zu `story="ERZÄHLERIN` ändern. Falls du an allen anderen
     stellen als Speaker `story` angegeben hast, wird von nun an nicht mehr
     "ERZÄHLER", sondern "ERZÄHLERIN" angezeigt, ohne, dass du sämtliche
     Speaker-Felder anpassen musstest. 
   - config-attribute lassen sich auch während dem Spiel ändern, d.h. du
     könntest während das Spiel läuft in einem bestimmten Szenario die den
     Erzähler ändern indem du das event `changePrinting story|ERZÄHLERIN`
     ausfürst. Ab dann werden dann alle `story` Sprecher als "ERZÄHLERIN"
     angezeigt. 

*HINWEIS*: du bist keinesfalls nur an die obigen vier Kategorien gebunden. Fühl
dich frei auch andere hinzuzufügen, um die oben genannten Vorteile zu nutzen. 

*HINWEIS*: Es gibt zwei andere automatische Ersetzungen: 
1. Du kannst in jedem Text `{name}` schreiben (in geschweiften Klammern), dann
   wird `{name}` ersetzt durch den Namen der Spieler:in. (Gibst du stattdessen
   `{NAME}` an wird der Name der Spieler:in in GROẞBUCHSTABEN ausgegeben)
2. Du kannst in jedem Dialog `{cname}` (character-name) schreiben, dann wird
   `{cname}` ersetzt durch den Namen der aktuellen Gesprächspartner:in. (Gibst du 
   stattdessen `{CNAME}` an wird der Name der Gesprächspartner:in GROẞBUCHSTABEN 
   ausgegeben)

