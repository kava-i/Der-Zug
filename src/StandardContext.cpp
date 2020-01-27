#include "CStandardContext.hpp"
#include "CPlayer.hpp"


// ***** PARSER ***** // 

vector<CContext::event> CStandardContext::parser(std::string sInput, CPlayer* p)
{
    std::cout << "standardParser: " << sInput << std::endl;
    //Create regular expressions for different command the player might have choosen
    std::regex show("(show) (.*)");
    std::regex examine("examine");
    std::regex lookIn("(look in )(.*)");
    std::regex pickUp("(pick up )(.*)");
    std::regex consume("(drink|eat|smoke) (.*)");
    std::regex equipe("(equipe) (.*)");
    std::regex dequipe("(dequipe) (.*)");
    std::regex goTo("(go to) (.*)");
    std::regex talkTo("(talk to) (.*)");
    std::regex help("help");
    std::regex end_direct(":q");
    std::regex tryMe("(try) (.*)");
    //Create an instans of smatch
    std::smatch m;

    //Show 
    if(std::regex_search(sInput, m, show))
        return {std::make_pair("show", m[2])};
    //Examine
    else if(std::regex_match(sInput, examine))
        return {std::make_pair("examine", "")};
    //Look in
    else if(std::regex_match(sInput, m, lookIn))
        return {std::make_pair("lookIn", m[2])};
    //Take
    else if(std::regex_match(sInput, m, pickUp))
        return {std::make_pair("take", m[2])}; 
    //Consume
    else if(std::regex_match(sInput, m, consume))
        return {std::make_pair("consume", m[2])};
    //Equipe
    else if(std::regex_match(sInput, m, equipe))
        return {std::make_pair("equipe", m[2])};
    //Dequipe
    else if(std::regex_match(sInput, m, dequipe))
        return {std::make_pair("dequipe", m[2])};
    //Change room
    else if(std::regex_match(sInput, m, goTo))
        return {std::make_pair("goTo", m[2])};
    //Talk to 
    else if(std::regex_match(sInput, m, talkTo))
        return {std::make_pair("talkTo", m[2])};
    //Help 
    else if(std::regex_match(sInput, help))
        return {std::make_pair("help", "standard.txt")};
    //Developer option
    else if(std::regex_match(sInput, m, tryMe))
        return {std::make_pair("try", m[2])}; 
    //Tutorial
    else if(sInput == "startTutorial")
        return {std::make_pair("startTutorial", "")};
    else
        return {std::make_pair("error", "")};
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
    else if(sIdentifier == "stats")
        p->appendPrint(p->showStats());
    else if(sIdentifier == "all")
        p->appendPrint(p->getRoom()->showAll());
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
