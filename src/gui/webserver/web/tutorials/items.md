# Items 

## Area/ SubCategory
Die Sub-Kategorie, oder "Area", d.h. die Einträge, die du siehst, nachdem du auf
"items" klickst, ist hier - wie überall anders auch - nur zu deiner eigenen
Orientierung gedacht: Hier kannst du beliebige Sub-Kategorien anlegen, die in
keiner Weise die Funktionen der in Ihnen erstellten Items betrifft, sondern nur
dir dabei helfen soll den Überblick zu behalten.

## Attribute 

### Categroy
Die Kategorie (`categroy`) bestimmt a) grundsätzliche Funktionen: `consume`,
`read` und `equipe`, die also bestimmen, ob der entsprechende Gegenstand
konsumiert werden kann (wie z.B. ein Trank oder Essen), gelesen werden kann
(z.B. ein Brief oder Buch) oder ob er ausgerüstet werden kann (wie z.B. eine
Waffe oder Rüstung), die Kategorie bestimmt aber auch b) wo der Gegenstand im
Inventar angezeigt werden soll. 

Du kannst über die config neue Kategorien (neben `consume`, `read` und `equipe`)
anlegen, diese erhalten dann aber **keine** eigenen oder neuen Funktionen,
sondern dienen einfach nur deiner Eigenen Sortierung

## Kind 
Die Art (`kind`) bestimmt eine Untergruppe (z.B. "weapon"/"armor" in der Kategorie 
"equipe", oder "Heiltrank"/"Gifttrank"/"Essen" in der Kategorie "consume"). 
Sie wird z.B. genutzt um deine ausgerüsteten Gegenstände genauer anzugeben, oder
vordefinierte Funktionen anzuwenden.

Du kannst über die config neue Arten definieren.

## Type 
Der Typ (`type`) ist rein benutzerdefiniert, könnte z.B. sowas wie "Feuer",
"Wasser", "Erde", "Luft" sein. 

Du kannst über die config neue Typen definieren. Für Attacken gelten automatisch
die gleichen Typen, wie für Gegenstände.

