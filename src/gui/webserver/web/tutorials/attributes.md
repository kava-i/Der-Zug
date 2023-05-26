# Attribute

Neben [Minds](/minds.md) sind Attribute die Möglichkeit den Status der
Spieler:in zu verändern. Während *minds* etwas specieller sind und dein Spiel
gut ohne sie auskommen kann, sollten dir Attribute bekannter sein. Sie können so
etwas, wie die Lebenspunkte, die Erfahrungspunkte oder die Geschicklichkeit der
Spieler:in bestimmen. Aber es ist auch möglich bestimmte Status-Eigenschaften
des Charakters zu setzen, z.B. ob dein Charakter blind ist, vergiftet ist,
raucher:in ist etc. 

Dabei ist die generell völlige Freiheit gelassen. Das Spiel setzt kein einziges
Attribut voraus und du kannst alle von uns "vorgeschlagenen" Attribute löschen
und das Spiel bleibt dennoch spielbar. 
Unter der Categorie ["Globale Listener"](/global_listners.md) kannst du selbst
bestimmen, welche Eigenschaften dauerhaft das Spiel beeinflussen (z.B. dass das
Spiel endet sobald die Lebenspunkte auf 0 gesunken sind. Aber auch jedes andere
Attribute könnte diese Funktion übernehmen. Du könntest ein Spiel erstellen
indem das einzige Attibute "geld" ist, die Menge des Geldes deine
Erfahrungspunkte darstellen, Level aufstiege ermöglichen und das Spiel endet,
sobald die Spieler:in kein Geld mehr hat). 

Aber eins nach dem anderen. 

Es gibt drei Unterkategorien: *attributes*, *config* und *mapping*.

## *Attributes* 
Hier bestimmst du alle Attribute, die in deinem Spiel existieren sollen. 
Dabei kannst du einen Default-Wert bestimmen, sowie den einzelnen Attributen
Kategorien zuweisen. 
Wenn eine Spieler:in sich während des Spiels die Attribute anzeigen lässt mit
dem Befehel: 
```
show stats 
```
bzw. 
```
zeige attribute
```
Werden die Attribute nach den Kategorien sortiert. 

*HINWEIS*: Alle Categorien, die mit einem Unterstrich beginnen, werden nicht
angezeigt, wenn die Spieler:in `zeige attribute` eingibt.
These can be used to create timer.

### Limits 
Außerdem kannst du für jedes Attribut `bounds` (`upper-bound`, `lower-bound`) 
definieren, limits erfüllen immer die Funktionen:
- der Wert wird niemals unter, bzw. über das angegebene Limit gesetzt
- wenn geforderte Attribute mit den aktuellen abgeglichen werden, um eine Aktion
  zu ermöglichen, wird diese nicht durchgeführt, mit der Meldung, dass ein
  Attribute unter/ über das angegebene limit gestiegen ist. 

Wenn außerdem `events` angegeben wurden, wird zursätzlich das event
durchgeführt.

## *Config*

### Skillable Categories
Hier kannst du einige Feinheiten einstellen. Beispielsweise welche Attribute bei
einem Level-Aufstieg "skillbar" sind. Dazu wählst du einfach einige Kategorien
und alle Attribute in diesen Kategorien können dann bei einem Level-Aufstieg
aufgewertet werden. 

*Hinweis*: auch hier gilt, dass diese Aktion nicht von einem Level-Aufstieg im
eigentlichen Sinne abhängig ist. Obwohl das der default ist, kannst du jederzeit
das event `skill <beliebige nummer>` aufrufen, wodurch die Spieler:in die
entsprechende anzahl an "Punkten" auf ihre Attribute verteilen kann. 

### Woozyness
Woozyness ist eine Spiel-interne mechanik. Jenachdem wie hoch die aktuelle
Woozyness ist werden die geschriebenen Wörter mehr oder weniger wild
durcheinander geworfen. 
Dazu legst du zunächst fest, welche Attribute die Woozyness beeinflussen (z.B.
könntest du -- um beim obigen Geld beispiel zu bleiben -- Woozyness von Geld
abhängig machen. D.h. so mehr Geld die Spieler:in sammelt, so mehr "stärker"
wird sie, aber zugleich wird der Text immer unlesbarer, was eine spannende
Dynamik erzeugen könnte). 
Es können auch mehrere Attribute kombiniert werden (default: Trunkenheit und
Highness) und du kannst selbst entscheiden wie diese sich gegenseitig
beeinflussen, z.B. ob der Durchschnitt gebildet wird oder die Werte
zusaammengerechnet werden. 

### Level Based 
Aktuell leider noch nicht implementiert. Die Idee ist aber eine automatische
Methode einzubauen, wodurch Attribute sich bei jedem Level-Aufstieg von selbst
erhöhen. 

## Mapping 
Attribute werden nur als Zahlenwerte dargestellt. Allerdings kann es sein, dass
das nicht der Ästhetik gerecht wird, die du vor Augen hast. Während also die
Attribute "im Hintergrund" stets Zahlen bleiben, kannst du hier bestimmte Zahlen
auf text "mappen". 
In unserem Beispeil (den default Einstellungen) wird z.B. für das Attribute
"Raucher:in* nie der Zahlenwert angezeigt, sondern eine der Beschreibungen
"Nicht-Raucher", "Raucher" oder "Kettenraucher" je nach dem aktuellen Zahlenwert 

- 0 → Nicht-Raucher 
- < 5 → Raucher 
- > 5 → Kettenraucher 

*Hinweis*: Hier bei ist die Reihenfolge bei der Auswertung entscheidend. Fange
stets mit dem kleinsten Wert an. 

## Attribute verändern 
Es gibt einige Möglichkeiten Attribute zu verändern. 

### Text-Felder 
Jedes Textfeld enthält ein `Update` Feld. Dadurch kann **jeder Text**, welcher
der Spieler:in angezeigt wird die Attribute (und Minds) der Spieler:in
verändern. 

Dabei kannst du beliebig viele Attribute verändern, undzar fix setzen (z.B.
`=10`), erhöhen (Addieren/ Multiplizieren, z.B. `+10` oder `*2`) oder senken
(Subtrahieren/ Dividieren, z.B. `-3` oder `/2`).

*Hinweis*: `Updates` via Texte werden nur ein einizges mal ausgeführt.

Hier kannst du außerdem `sekunden` angeben. Bei 0 Sekunden wird das Attribute
"für immer" verändert. 
Ansonsten wird ein Time-Event gestartet, welches entweder a) das Attribute nach
xxx Sekunden wieder auf den alten Wert zurücksetzt, bzw. falls das Attribute
z.B. um 10 erhöhrt wird, nach xxx Sekunden wieder um 10 senkt.


### `Updates` Feld
Zusätzlich enthalten die Folgenden Objekte ebenfalls Update-Felder: 
- Quests (wird einmalig ausgeführt, sobald die Quest erfolgreich ist 
- Quest-Steps (wird einmalig ausgeführt, sobald die entsprechende "Etappe" der
  Quest abgeschlossen ist. 
- Charakters (Personen) (wird einmalig ausgeführt, wenn der Charakter in einem
  Kampf besiegt wurde)
- Items (Items mit der Categorie `equipe` führen die Updates einmal durch, wenn
  sie ausgerüstet werden und machen sie wieder rückgängig, wenn sie abgerüstet
  werden. Items mit der Categorie `consume` oder `read` führen die Updates einmalig 
  beim Konsumieren bzw. Lesen durch. Items anderer Kategorien führen
  die Updates *nicht* durch. Aber du kannst bei der `use-description`, die
  ausgegeben wird, wenn `use <name-des-items>` von der Spieler:in eingegeben
  wird, das `setAttibute` Event werfen oder das `Update`-Feld dieses Texts
  nutzen.

### Events/ Handler 
Außerdem können einzelne Attribute auch mit dem Handler `h_setAttribute` oder
dem Event `setAttibute` verändert werden. 

Auch hier kannst du setzen, erhöhren oder verringern, wie beim Update-Feld. 

Schließlich kannst du mit dem Event `tempSetAttribute` das Attribut für eine
gewisse Zeit verändern.

*Hinweis*: Der Unterschied zwischen Handlern und Events ist, dass ein Handler
nicht "geworfen", sondern direkt ausgeführt wird. D.h. auf ein Event können auch
noch andere Listener reagieren und wieder andere Handler "werfen". Bei Handlern
ist das nicht möglich.

## Auf Veränderungen in den Attributen reagieren
Alle dieser Aktionen "werfen" ein neues Event `attribute_set
<attribute_name>|<neuer_wert>`. Mit den globalen Handlern oder Quest-Handler
kannst du auf diese Attribute-Änderungen reagieren. Z.B. kannst du einen
globalen Händer erstellen, der das Spiel beendet, sobald das Attribute `Geld`
auf 0 gesunken ist.
