# Items 

## Area/ SubCategory
Die Sub-Kategorie, oder "Area", d.h. die Einträge, die du siehst, nachdem du auf
"items" klickst, ist hier -- wie überall anders auch -- nur zu deiner eigenen
Orientierung gedacht: Hier kannst du beliebige Sub-Kategorien anlegen, die in
keiner Weise die Funktionen der in Ihnen erstellten Items betrifft, sondern nur
dir dabei helfen soll den Überblick zu behalten.

Sinnvolle Subkategorien könnten z.B. soetwas sein wie "Quest-Items",
"Ausrüstung", "Essen", ...

## Attribute 

### Categroy
Die Kategorie (`categroy`) bestimmt a) grundsätzliche Funktionen: `consume`,
`read` und `equipe`, die also bestimmen, ob der entsprechende Gegenstand
konsumiert werden kann (wie z.B. ein Trank oder Essen), gelesen werden kann
(z.B. ein Brief oder Buch) oder ob er ausgerüstet werden kann (wie z.B. eine
Waffe, Kleidung oder Schmuck), die Kategorie bestimmt aber auch b) wo der 
Gegenstand im Inventar angezeigt werden soll. 

Du kannst über die Config neue Kategorien (neben `consume`, `read` und `equipe`)
anlegen, diese erhalten dann aber **keine** eigenen oder neuen Funktionen,
sondern dienen einfach nur deiner eigenen Sortierung und der Anzeige im Inventar

### Kind 
Die Unterart (`kind`) bestimmt eine Untergruppe. Dabei gehören Unterarten immer zur
Kategory, d.h. du kannst für unterschliedliche Kategorien verschiedene
Unterarten angeben.

*HINWEIS*: Diese Unterarten haben **keine Spiel-Funktionen** außer im Fall von Ausrüstung:
du kannst nicht zwei Gegenstände mit der gleichen Unterart ausgerüstet haben.

### Type 
Der Typ (`type`) ist rein benutzerdefiniert, könnte z.B. sowas wie "Feuer",
"Wasser", "Erde", "Luft" sein. 

Du kannst über die config neue Typen definieren. Für Attacken gelten automatisch
die gleichen Typen, wie für Gegenstände.

# Nutzen von Items
Wenn die Spieler:in `use <item>` eingibt wird (bei Erfolg) *immer* die
UseDescription ausgegeben. D.h. darüber können Events geworfen werden. (Es
empfiehlt sich hier nur permanent events zu verwenden!).

*HINWEIS*: Kein Erfolg gibt es nur, wenn ein Item ausgerüstet werden soll,
welches nicht ausgerüstet werden kann, da beireits ein Item mit der gleichen
Unterart definiert ist. 

Außerdem werden der `categroy` entsprechenden Aktionen ausgeführt (Ausrüstung,
Konsumieren, Lesen):
- **equipe**: Der Gegenstand wird ausgerüstet, `updates` werden angewendet 
- **dequipe**: Der Gegenstand wird abgerüstet, `updates` werden rückgängig gemacht 
- **read**: Du kannst das Buch lesen. `updates` werden sofort durchgeführt und
  danach gelöscht (d.h. nicht nochmal durchgeführt, wenn das Buch nochmal
  gelesen werden soll)
- **consume**: Der gegenstand wird konsumiert, `updates` werden durchegführt,
  das item wird danach aus dem Inventar eventfernt. 

Updates für alle anderen Kategorien kannst du trotzdem über `updates`
der `use-description` durchführen lassen, sowie events schmeißen.

Andere funktionen, kannst du immer über die `permantent-events` der
`use-description` durchführen. Theoretisch könnte z.B. immer eine Person
erscheinen, wenn du das Items nutzt, indem du das event `startDialogDirect` bei
der use-description wirfst.
