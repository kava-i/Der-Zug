#include "CStandardContext.hpp"
#include "CPlayer.hpp"

CStandardContext::CStandardContext()
{
    //Set permeability
    m_permeable = true;

    //Add listeners
    add_listener("show", &CContext::h_show);
    add_listener("examine", &CContext::h_examine);
    add_listener("lookIn", &CContext::h_lookIn);
    add_listener("goTo", &CContext::h_goTo);
    add_listener("talkTo", &CContext::h_startDialog);
    add_listener("take", &CContext::h_take);
    add_listener("consume", &CContext::h_consume);
    add_listener("equipe", &CContext::h_equipe);
    add_listener("dequipe", &CContext::h_dequipe);
    add_listener("help", &CContext::h_help);
    add_listener("error", &CContext::h_error);

    //Rooms
    add_listener("goTo", &CContext::h_firstZombieAttack);
    add_listener("goTo", &CContext::h_moveToHospital, 0);
    add_listener("goTo", &CContext::h_endTutorial);

    //Tutorial
    add_listener("startTutorial", &CContext::h_startTutorial);

    //Developer
    add_listener("try", &CContext::h_try, 0);
}


// **** HANDLER **** //

void CStandardContext::h_show(string& sIdentifier, CPlayer* p) {
    if(sIdentifier == "exits")
        p->appendPrint(p->getRoom()->showExits());
    else if(sIdentifier == "people")
        p->appendPrint(p->getRoom()->showCharacters());
    else if(sIdentifier == "room")
        p->appendPrint(p->getRoom()->showDescription(p->getWorld()->getCharacters()));
    else if(sIdentifier == "items")
        p->appendPrint(p->getRoom()->showItems());
    else if(sIdentifier == "details")
        p->appendPrint(p->getRoom()->showDetails());
    else if(sIdentifier == "inventory")
        p->printInventory();
    else if(sIdentifier == "equiped")
        p->printEquiped();
    else if(sIdentifier == "quests")
        p->showQuests();
    else if(sIdentifier == "stats")
        p->appendPrint(p->showStats());
    else if(sIdentifier == "all")
        p->appendPrint(p->getRoom()->showAll());
    else
        p->appendPrint("Unkown \"show-function\"\n"); 
}

void CStandardContext::h_examine(string& sIdentifier, CPlayer* p) {
    p->appendPrint(p->getRoom()->showAll());
}

void CStandardContext::h_lookIn(string& sIdentifier, CPlayer* p) {
    string sOutput = p->getRoom()->look("in", sIdentifier);
    if(sOutput == "")
        p->appendPrint("Nothing found. \n");
    else
        p->appendPrint(sOutput);
}

void CStandardContext::h_goTo(std::string& sIdentifier, CPlayer* p) {
    p->changeRoom(sIdentifier);
}

void CStandardContext::h_startDialog(string& sIdentifier, CPlayer* p)
{
    //Get selected character
    string character = p->getObject(p->getRoom()->getCharacters(), sIdentifier);
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
    if(p->getRoom()->getItem(sIdentifier) == NULL)
        p->appendPrint("Item not found.\n");
    else
        p->addItem(p->getRoom()->getItem(sIdentifier));
}

void CStandardContext::h_consume(string& sIdentifier, CPlayer* p) {
    if(p->getItem(sIdentifier) != NULL) {
        if(p->getItem(sIdentifier)->callFunction(p) == false)
            p->appendPrint("This item is not consumeable.\n");
    }
}

void CStandardContext::h_equipe(string& sIdentifier, CPlayer* p) {
    if(p->getItem(sIdentifier) != NULL) {
        if(p->getItem(sIdentifier)->callFunction(p) == false)
            p->appendPrint("This item is not equipable.\n");
    }
}

void CStandardContext::h_dequipe(string& sIdentifier, CPlayer* p) {
    p->dequipeItem(sIdentifier);
}

void CStandardContext::h_error(string& sIdentifier, CPlayer* p) {
    p->appendPrint("This command is unkown. Type \"help\" to see possible command.\n");
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
    delete_listener("goTo", 2);
}

void CStandardContext::h_moveToHospital(string& sIdentifier, CPlayer* p)
{
    std::cout << "h_moveToHospital: " << sIdentifier << std::endl;
    //Get selected room
    if(p->getRoom()->getID().find("compartment") == std::string::npos)
        return;

    sIdentifier = "Foyer";
    p->setRoom(p->getWorld()->getRooms()["hospital_stairs"]);
}

void CStandardContext::h_endTutorial(string& sIdentifier, CPlayer* p)
{
    std::cout << "h_endTutorial: " << sIdentifier << std::endl;
    if(p->getRoom()->getID().find("gleis3") == std::string::npos)
        return;
    if(p->getItem_byID("ticket") == NULL)
    {
        std::cout << "Couldn't find ticket.\n";
        return;
    }

    p->appendPrint("Du siehst deinen Zug einfahren. Du bewegst dich auf ihn zu, zeigst dein Ticket, der Schaffner musstert dich kurz und lässt dich dann eintreten. Du suchst dir einen freien Platz, legst dein Bündel auf den sitz neben dich und schläfst ein...\n $ Nach einem scheinbar endlos langem schlaf wachst du wieder in deinem Abteil auf. Das Abteil ist leer. Leer bist auf einen geheimnisvollen Begleiter: Parsen.");

    p->setRoom(p->getWorld()->getRooms()["compartment-a"]);

}

void CStandardContext::h_startTutorial(string&, CPlayer* p)
{
    p->appendPrint("Willkommen bei \"DER ZUG\"! Du befindest dich auf dem Weg nach Moskau. Dir fehlt dein Ticket. Tickets sind teuer. Glücklicherweise kennst du einen leicht verrückten, viel denkenden Mann, der sich \"Der Ticketverkäufer\" nennt. Suche ihn, er hat immer noch ein günsttiges Ticket für dich. Benutze die Befhelte \"go to [name des Ausgangs]\", um den Raum zu wechseln, um dir Personen und Ausgänge anzeigen zu lassen, nutze \"show people\", bzw. \"show exits\" oder auch \"show all\". Eine Liste mit allen Befhelen und zusätzlichen Hilfestellungen erhältst du, indem du \"help\" eingibst.\n $\n");

    p->appendPrint(p->getRoom()->getDescription());
}

void CStandardContext::h_try(string& sIdentifier, CPlayer* p)
{
   
}
