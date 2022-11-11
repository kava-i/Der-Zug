# Dialoge 

Es gibt drei arten von Dialogen:
1. den `defaultDialog` (der muss so heißen und muss angelegt werden!)
2. die default-dialogs, die jeweils nach eine Kategorie von Dialogen benannt
   sind, z.B. `default_mosters` oder `default_passanten`
3. die Charakter-Dialoge, die jeweils nach dem entsprechenden Charakter benannt
   sind. 

Während es nur einen einzigen `defaultDialog` gibt, der für alle Charaktere
genutzt wird, die weder einen eigenen Dialog haben, noch einer
default Dialog-Gruppe zugeordnet sind (eben z.B. `default_mosters` oder
`default_passanten`), sind die jeweils anderen Dialoge nummeriert, bzw. müssen
nummeriert sein. Z.B. 
- default_passanten_1
- default_passanten_2
- default_mosters_1
- default_mosters_2
- default_mosters_3

Oder wenn es z.B. einen Charakter namens `anna` gibt: 
- anna_1 
- anna_2
- ...

Im Fall der default Dialoge wird zufällig einer ausgewählt. Bei den
Charakter-Dialogen wird immer der erste verwendet. Dieser kann dann aber im Lauf
des Spiels durch den 2., 3., 4., ... ersetzt werden, je nachdem, was die
Spieler:in für Entscheidungen trifft
