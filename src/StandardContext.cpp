#include "CStandardContext.hpp"
#include "CPlayer.hpp"

CStandardContext::CStandardContext()
{
    //Set permeability
    m_permeable = false;
    m_curPermeable = m_permeable;

    //Set help
    m_sHelp = "standard.txt";

    //Add listeners
    add_listener("show", &CContext::h_show);
    add_listener("look", &CContext::h_look);
    add_listener("go", &CContext::h_goTo);
    add_listener("talk", &CContext::h_startDialog);
    add_listener("pick", &CContext::h_take);
    add_listener("consume", &CContext::h_consume);
    add_listener("equipe", &CContext::h_equipe);
    add_listener("dequipe", &CContext::h_dequipe);
    add_listener("examine", &CContext::h_examine);
    add_listener("mode", &CContext::h_changeMode);
    add_listener("help", &CContext::h_help);

    add_listener("find", &CContext::h_find);

    //Rooms
    add_listener("go", &CContext::h_firstZombieAttack);
    add_listener("go", &CContext::h_moveToHospital, 0);
    add_listener("go", &CContext::h_exitTrainstation, 0);

    //Tutorial
    add_listener("startTutorial", &CContext::h_startTutorial);

    //Developer
    add_listener("try", &CContext::h_try, 0);
}

void CStandardContext::h_find(string& sIdentifier, CPlayer*p) {

    auto lamda = [](CRoom* room) { return room->getName(); }; 
    std::string roomID = func::getObjectId(p->getWorld()->getRooms(), sIdentifier, lamda);

    std::vector<std::string> path = p->findWay(p->getRoom(), roomID);
    if(path.size() > 0) {
        p->appendPrint("Path to " + sIdentifier + ": \n");
        for(const auto& it : path)
            p->appendPrint(it + "\n");
    }
    else
        p->appendPrint("Room not found.\n");
}

// **** HANDLER **** //

void CStandardContext::h_show(string& sIdentifier, CPlayer* p) {
    if(sIdentifier == "exits")
    {
        p->appendPrint(p->getRoom()->showExits(p->getMode()));
        p->addSelectContest(p->getRoom()->getExtits2(), "go");
    }
    else if(sIdentifier == "people")
    {
        p->appendPrint(p->getRoom()->showCharacters(p->getMode()));
        p->addSelectContest(p->getRoom()->getCharacters(), "talk");
    }
    else if(sIdentifier == "room")
        p->appendPrint(p->getRoom()->showDescription());
    else if(sIdentifier == "items")
        p->appendPrint(p->getRoom()->showItems(p->getMode()));
    else if(sIdentifier == "details")
        p->appendPrint(p->getRoom()->showDetails(p->getMode()));
    else if(sIdentifier == "inventory")
        p->appendPrint(p->getInventory().printInventory());
    else if(sIdentifier == "equiped")
        p->printEquiped();
    else if(sIdentifier == "quests")
        p->showQuests(false);
    else if(sIdentifier == "solved quests")
        p->showQuests(true);
    else if(sIdentifier == "stats")
        p->showStats();
    else if(sIdentifier == "mind")
        p->showMinds();
    else if(sIdentifier == "attacks")
        p->appendPrint(p->printAttacks());
    else if(sIdentifier == "all")
        p->appendPrint(p->getRoom()->showAll(p->getMode()));
    else
        p->appendPrint("Unkown \"show-function\"\n"); 
}

void CStandardContext::h_look(string& sIdentifier, CPlayer* p) {
    size_t pos=sIdentifier.find(" ");
    if(pos == std::string::npos) {
        p->appendPrint("Soll ich in, auf oder unter der box schauen?\n");
        return;
    }

    p->appendPrint(p->getRoom()->look(sIdentifier.substr(0,pos),sIdentifier.substr(pos+1),p->getMode()));
}

void CStandardContext::h_goTo(std::string& sIdentifier, CPlayer* p) {
    p->changeRoom(sIdentifier);
}

void CStandardContext::h_startDialog(string& sIdentifier, CPlayer* p)
{
    //Get selected character
    string character = func::getObjectId(p->getRoom()->getCharacters(), sIdentifier);
    CPlayer* player = p->getPlayer(sIdentifier);

    //Check if character was found
    if(character != "") 
        p->startDialog(character);
    else if(player != NULL) 
        p->startChat(player);
    else
        p->appendPrint("Character not found");
}

void CStandardContext::h_take(string& sIdentifier, CPlayer* p) {
    if(sIdentifier.find("all") == 0)
        p->addAll();
    else if(p->getRoom()->getItem(sIdentifier) == NULL)
        p->appendPrint("Item not found.\n");
    else
        p->addItem(p->getRoom()->getItem(sIdentifier));
}

void CStandardContext::h_consume(string& sIdentifier, CPlayer* p) {
    if(p->getInventory().getItem(sIdentifier) != NULL) {
        if(p->getInventory().getItem(sIdentifier)->callFunction(p) == false)
            p->appendPrint("This item is not consumeable.\n");
    }
    else
        p->appendPrint("Item not in inventory! (use \"show inventory\" to see your items.)\n");
}

void CStandardContext::h_equipe(string& sIdentifier, CPlayer* p) {
    if(p->getInventory().getItem(sIdentifier) != NULL) {
        std::cout << "Function: " << p->getInventory().getItem(sIdentifier)->getFunction() << std::endl;
        if(p->getInventory().getItem(sIdentifier)->callFunction(p) == false)
            p->appendPrint("This item is not equipable.\n");
    }
}

void CStandardContext::h_dequipe(string& sIdentifier, CPlayer* p) {
    p->dequipeItem(sIdentifier);
}

void CStandardContext::h_examine(string &sIdentifier, CPlayer*p) {
    //Check for detail
    auto lambda1 = [](CDetail* detail) { return detail->getName(); };
    std::string sObject = func::getObjectId(p->getRoom()->getDetails(), sIdentifier, lambda1);
    if(sObject != "")
        p->appendPrint(p->getRoom()->getDetails()[sObject]->getDescription());

    auto lambda2 = [](CItem* item) { return item->getName(); };
    sObject = func::getObjectId(p->getRoom()->getItems(), sIdentifier, lambda2);
    //Check for item
    if(sObject != "")
        p->appendPrint(p->getRoom()->getItems()[sObject]->getDescription());

    //Check for person
    sObject = func::getObjectId(p->getRoom()->getCharacters(), sIdentifier);
    //Check for item
    if(sObject != "")
        p->appendPrint(p->getWorld()->getCharacters()[sObject]->getDescription());
}

void CStandardContext::h_changeMode(string& sIdentifier, CPlayer* p) {
    p->changeMode();
}

void CStandardContext::error(CPlayer* p) {
    p->appendPrint("Standard: This command is unkown. Type \"help\" to see possible command.\n");
}


// **** SPECIAL HANDLER ***** //
void CStandardContext::h_firstZombieAttack(string& sIdentifier, CPlayer* p)
{
    //Get selected room
    if(p->getRoom()->getID() != "hospital_stairs")
        return;

    p->appendPrint("\n$\nA zombie comes running down the stairs and attacks you!");

    //Create fight
    p->setFight(new CFight(p, p->getWorld()->getCharacters()["hospital_zombie1"]));
    delete_listener("go", 3);
}

void CStandardContext::h_moveToHospital(string& sIdentifier, CPlayer* p)
{
    //Get selected room
    if(p->getRoom()->getID().find("compartment") == std::string::npos || fuzzy::fuzzy_cmp("corridor", sIdentifier) > 0.2)
        return;

    p->changeRoom(p->getWorld()->getRooms()["hospital_foyer"]);
    m_block = true;
}

void CStandardContext::h_exitTrainstation(string& sIdentifier, CPlayer* p)
{
    if(p->getRoom()->getID() != "bahnhof_eingangshalle" || func::getObjectId(p->getRoom()->getExtits2(), sIdentifier) != "ausgang")
        return;

    p->appendPrint("Du drehst dich zum dem großen, offen stehenden Eingangstor der Bahnhofshalle. Und kurz kommt dir der Gedanke doch den Zug nicht zu nehmen, doch alles beim Alten zu belassen. Doch etwas sagt dir, dass es einen guten Grund gab das nicht zu tun, einen guten Grund nach Moskau zu fahren. Und auch, wenn du ihn gerade nicht mehr erkennst. Vielleicht ist gerade das der beste Grund: rausfinden, was dich dazu getrieben hat, diesen termin in Moskau zu vereinbaren.\n Du guckst dich wieder in der Halle um, und überlegst, wo du anfängst zu suchen.\n");
    
    m_block=true;
}

void CStandardContext::h_startTutorial(string&, CPlayer* p)
{
    p->appendPrint("Willkommen bei \"DER ZUG\"! Du befindest dich auf dem Weg nach Moskau. Dir fehlt dein Ticket. Tickets sind teuer. Glücklicherweise kennst du einen leicht verrückten, viel denkenden Mann, der sich \"Der Ticketverkäufer\" nennt. Suche ihn, er hat immer noch ein günsttiges Ticket für dich. Benutze die Befhelte \"go to [name des Ausgangs]\", um den Raum zu wechseln, um dir Personen und Ausgänge anzeigen zu lassen, nutze \"show people\", bzw. \"show exits\" oder auch \"show all\". Eine Liste mit allen Befhelen und zusätzlichen Hilfestellungen erhältst du, indem du \"help\" eingibst.\n $\n");

    p->appendPrint(p->getRoom()->getDescription() + "\n");
    p->setNewQuest("zug_nach_moskau");
    p->setNewQuest("tutorial");
}

void CStandardContext::h_try(string& sIdentifier, CPlayer* p)
{
   
}
