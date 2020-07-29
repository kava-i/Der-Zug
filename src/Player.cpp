#include "CPlayer.hpp"

#define GREEN Webcmd::set_color(Webcmd::color::GREEN)
#define RED Webcmd::set_color(Webcmd::color::RED)
#define BLUE Webcmd::set_color(Webcmd::color::BLUE)
#define PURPLE Webcmd::set_color(Webcmd::color::PURPLE)
#define WHITE Webcmd::set_color(Webcmd::color::WHITE)
#define WHITEDARK Webcmd::set_color(Webcmd::color::WHITEDARK)
#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

/**
* Full constructor for player
* @param jAtts json with all attributes
* @param room current room of payer
* @param newAttacks attacks of player
*/
CPlayer::CPlayer(nlohmann::json jAtts, CRoom* room, attacks lAttacks, CGramma* gramma) : CPerson(jAtts, nullptr, lAttacks, nullptr, this, std::map<std::string, CDialog*>())
{
    //Set login data and player information
    func::convertToUpper(m_sName);
    m_sPassword = jAtts["password"];
    m_firstLogin = true; 
    m_sMode = jAtts.value("mode", "prosa"); 
    m_abbilities = {"strength", "skill"};

    //Initiazize world
    std::cout << "Creating world.\n";
    m_world = new CWorld(this);
    std::cout << "Done.\n";
    m_parser = new CParser(m_world->getConfig());
    m_gramma = gramma;
    

    //Character and Level
    m_level = 0;
    m_ep = 0;
    m_minds["perzeption"] = {"perzeption", BLUE, 1, 0.5};
    m_minds["peddler"] = {"peddler", RED, 0, 0.4};
    m_minds["drama"]  = {"drama", RED, 0, 0.3};
    m_minds["logik"]  = {"logik", RED, 0, 0.2};

    //Attributes
    if(m_world->getConfig().count("attributes")>0)
    {
        std::vector<std::string> attributes = m_world->getConfig()["attributes"];
        for(auto it : attributes)   
            m_stats[it] = 0;
    }

    //Set current room
    m_room = room;

    //Set player's equipment (no equipment at the beginning)
    m_equipment["weapon"] = NULL;
    m_equipment["armor"]  = NULL;
    
    //Initialize all rooms as not visited
    for(const auto& it : m_world->getRooms())
        m_vistited[it.second->getID()] = false;
    m_room = m_world->getRoom(m_room->getID());
    m_vistited[m_room->getID()] = true;


    //Initialize context stack
    for(auto it : m_world->getQuests())
    {
        CEnhancedContext* context = new CEnhancedContext((nlohmann::json){{"name", it.first}, {"permeable",true}, {"questID",it.first}});
        context->initializeHandlers(it.second->getHandler());
        m_contextStack.insert(context, 3, it.first);
    }

    //Add eventhandler to eventmanager
    m_contextStack.insert(new CEnhancedContext((std::string)"world"), 2, "world");
    CEnhancedContext* context = new CEnhancedContext((std::string)"standard");
    m_contextStack.insert(context, 0 , "standard");
    updateRoomContext();

    //Add quests
    if(jAtts.count("quests") > 0)
    {
        for(const auto& it : jAtts["quests"])
            setNewQuest(it);
    }
}

CPlayer::~CPlayer()
{
    delete m_world;
} 

// *** GETTER *** // 

///Return first login (yes, no)
bool CPlayer::getFirstLogin() { 
    return m_firstLogin; 
}

///Return mode (Prosa or List) 
std::string CPlayer::getMode() { 
    return m_sMode; 
}

///Return output for player (Append newline)
string CPlayer::getPrint()  { 
    checkCommands();
    return m_sPrint + "\n";
}

///Return pointer to players world (all rooms, chars, etc.)
CWorld* CPlayer::getWorld() { 
    return m_world; 
}

///Return pointer to gramma-class
CGramma* CPlayer::getGramma() {
    return m_gramma;
}

///Return current room.
CRoom* CPlayer::getRoom() { 
    return m_room; 
}

///Return dictionary of player's minds.
std::map<std::string, SMind>& CPlayer::getMinds() { 
    return m_minds; 
}

SMind& CPlayer::getMind(std::string sMind) {
    if(m_minds.count(func::returnToLower(sMind)) > 0)
        return m_minds[func::returnToLower(sMind)];
    std::cout << cRED << "FATAL Error! Requested mind does not exist: " << sMind << std::endl;
    return m_minds["logik"]; 
}

///Return players abilities (strength, moral, etc.)
std::vector<std::string> CPlayer::getAbbilities() { 
    return m_abbilities; 
}

///Return players equipment (weapons, clothing etc.)
std::map<std::string, CItem*>& CPlayer::getEquipment() { 
    return m_equipment; 
}

///Return current fight.
CFight* CPlayer::getFight() { 
    return m_curFight; 
}

//Return players context-stack 
CContextStack<CEnhancedContext>& CPlayer::getContexts() { 
    return m_contextStack; 
}

//Return map of players
std::map<std::string, CPlayer*>& CPlayer::getMapOFOnlinePlayers()
{
    return m_players;
}

// *** SETTER *** // 

///Set first login.
void CPlayer::setFirstLogin(bool val) { 
    m_firstLogin = val; 
}

///Set new output for player.
void CPlayer::setPrint(string newPrint) { 
    m_sPrint = newPrint; 
}

///Append to current player output and throw staged events if exists.
void CPlayer::appendPrint(std::string sPrint) 
{
    throw_staged_events(m_staged_pre_events, "pre");
    if(m_staged_pre_events != "")
        m_sPrint += "\n";
    m_sPrint += sPrint;
    throw_staged_events(m_staged_post_events, "post"); 
}

void CPlayer::appendStoryPrint(string sPrint) { 
    appendSpeackerPrint(m_world->getConfig()["printing"]["story"], sPrint);
}

void CPlayer::appendDescPrint(string sPrint) {
    appendSpeackerPrint(m_world->getConfig()["printing"]["desc"], sPrint);
}

void CPlayer::appendErrorPrint(string sPrint) {
    appendSpeackerPrint(m_world->getConfig()["printing"]["error"], sPrint);
}

void CPlayer::appendTechPrint(string sPrint) {
    appendSpeackerPrint(m_world->getConfig()["printing"]["tech"], sPrint);
}

void CPlayer::appendSpeackerPrint(std::string sSpeaker, std::string sPrint) {
    sPrint = func::returnSwapedString(sPrint, getStat("highness"));

    if(sSpeaker != "")
        appendPrint("<div class='spoken'>" + sSpeaker + " - " + WHITEDARK + sPrint + WHITE + "</div>");
    else
        appendPrint(sPrint + "\n");
}

std::string CPlayer::returnSpeakerPrint(std::string sSpeaker, std::string sPrint) {
    sPrint = func::returnSwapedString(sPrint, getStat("highness"));
    
    if(sSpeaker != "")
        return "<div class='spoken'>" + sSpeaker + " - " + WHITEDARK + sPrint + WHITE + "</div>";
    else
        return sPrint + "\n";
}

void CPlayer::appendSuccPrint(string sPrint) {
    m_sPrint += GREEN + sPrint + WHITE;
}

///Add staged events to throw before printing
void CPlayer::addPreEvent(std::string sNewEvent)
{
    if(m_staged_pre_events == "")
        m_staged_pre_events = sNewEvent;
    else
        m_staged_pre_events += ";"+sNewEvent;
}

///Add staged events to throw after printing
void CPlayer::addPostEvent(std::string sNewEvent)
{
    if(m_staged_post_events == "")
        m_staged_post_events = sNewEvent;
    else
        m_staged_post_events += ";"+sNewEvent;
}

//Throw staged events and clear events afterwards
void CPlayer::throw_staged_events(std::string& events, std::string sMessage) 
{
    if(events == "")
        return;

    std::string newEvents = events;
    events = "";
    throw_events(newEvents, sMessage+"-events"); 
}


///Set player's world.
void CPlayer::setWorld(CWorld* newWorld) { 
    m_world = newWorld; 
}

///Set last room to current room and current room to new room.
void CPlayer::setRoom(CRoom* room) { 
    m_lastRoom = m_room; m_room = room; 
}

///Set map of all online players.
void CPlayer::setPlayers(map<string, CPlayer*> players) { 
    m_players = players; 
}

///Set webconsole.
void CPlayer::setWobconsole(Webconsole* webconsole) { 
    _cout = webconsole; 
}


// *** *** FUNCTIONS *** *** // 


/**
* Update room context after changing location
*/
void CPlayer::updateRoomContext()
{
    m_contextStack.erase("room");
    CEnhancedContext* context = new CEnhancedContext((std::string)"room");
    for(auto it : m_room->getHandler())
    {
        context->add_listener(it);
        if(it.count("infos") > 0)
            context->getAttributes()["infos"][(std::string)it["id"]] = it["infos"];
    }
    m_contextStack.insert(context, 0, "room");
}

/**
* Change mode to 'prosa' or 'list' mode and print change
*/
void CPlayer::changeMode()
{
    if(m_sMode == "prosa")
        m_sMode = "list";
    else
        m_sMode = "prosa";
    appendTechPrint("\"Mode\" wurde auf '" + m_sMode + "' gesetzt.\n");
}

// *** Fight *** //

/**
* Set current fight of player and add a fight-context to context-stack
* @param newFight new fight
*/
void CPlayer::setFight(CFight* newFight) { 
    m_curFight = newFight;

    //Create fight-context and add to context-stack.
    CEnhancedContext* context = new CEnhancedContext((std::string)"fight");
    context->initializeFightListeners(getAttacks2());
    m_contextStack.insert(context, 1, "fight");

    m_curFight->initializeFight();
}

/**
* Delete the current fight and erase fight-context from context-stack
*/
void CPlayer::endFight() {
    delete m_curFight;
    m_contextStack.erase("fight");
    appendPrint("Fight ended.\n");
}

// *** Dialog *** //


/**
* Set current (new) Dialog player and add a Dialog-context to context-stack.
* @param sCharacter id of dialogpartner and throw event to start Dialog.
*/
void CPlayer::startDialog(string sCharacter, CDialog* dialog)
{
    if(dialog != nullptr)
        m_dialog = dialog;
    else
        m_dialog = m_world->getCharacter(sCharacter)->getDialog();

    //Create context and add to context-stack.
    CEnhancedContext* context = new CEnhancedContext((std::string)"dialog", {{"partner", sCharacter}});
    context->initializeDialogListeners("START", this);
    m_contextStack.insert(context, 1, "dialog");

    std::string newCommand = m_dialog->getState("START")->callState(this);
    if(newCommand != "")
        throw_events(newCommand, "CPlayer::startDialog");
}

/**
* Try to start chatting. If player is busy, print error message, else add chat-context to 
* context-stack and throw event 'Hey + player-name'.
*/
void CPlayer::startChat(CPlayer* player)
{
    appendTechPrint("Sorry this option is currently not available\n");
    appendStoryPrint("Du gehst auf " + player->getName() + " zu und räusperst dich... \n");

    if(player->getContexts().nonPermeableContextInList() == true)
        appendTechPrint(player->getName() + " ist zur Zeit beschäftigt.\n");
    else
    {
        //Add Chat context for both players
        addChatContext(player->getID());
        player->addChatContext(m_sID);
        
        //Send text by throwing event
        throw_events("Hey " + player->getName() + ".", "CPlayer::startChat");
    }
}


/**
* Direct print of message to web-console. Used when chatting and usually call in chat-context
* of the Dialog partner, printing, what he said.
* @param sMessage message to print to console
*/
void CPlayer::send(string sMessage)
{
    _cout->write(sMessage);
    _cout->flush(); 
}

// *** Room *** 

/**
* Check player input, whether 1. player wants to go back, 2. player wants to use an exit in current
* room, or 3. player wants to go to a room already visited.
* @param sIdentifier player input (room id, exit of current room, or room already visited)
*/
void CPlayer::changeRoom(string sIdentifier)
{
    //Check if player wants to go back.
    if(sIdentifier == "back" || sIdentifier == "zurück") {
        changeRoom(m_lastRoom);
        return;
    }

    //Get selected room, checking exits in current room.
    auto lamda1= [](CExit* exit) { return exit->getPrep() + " " + exit->getName(); };
    string room = func::getObjectId(getRoom()->getExtits(), sIdentifier, lamda1);

    if(room != "") {
        changeRoom(getWorld()->getRooms()[room]);
        return;
    }

    //Check all rooms already visited.
    auto lamda2 = [](CRoom* room) { return room->getName(); };
    room = func::getObjectId(m_world->getRooms(), sIdentifier, lamda2);
    std::vector<std::string> path = findWay(m_room, room);

    //If a path was found, add events to go to each room in path.
    if(path.size() > 0)
    {
        std::string events;
        for(const auto& it : path)
            events += "go " + it + ";";
        events.pop_back();
        throw_events(events, "CPlayer::changeRoom");
    }
    
    //Print error message.
    else
        appendErrorPrint("Room not found.\n");
}

/**
* Change room to given room and print entry description. Set last room to current room.
* @param newRoom new room the player changes to
*/
void CPlayer::changeRoom(CRoom* newRoom)
{
    m_lastRoom = m_room; 
    m_room = newRoom;
    std::string entry = newRoom->getEntry();
    if(entry != "")
        appendDescPrint(entry);
    appendPrint(m_room->showDescription(m_world->getCharacters()));
    m_vistited[m_room->getID()] = true;

    updateRoomContext();
}

/**
* Print all already visited rooms.
*/
void CPlayer::showVisited()
{
    std::map<std::string, std::string> mapRooms;
    for(auto const& it : m_vistited)
    {
        if(it.second == true)
            mapRooms[it.first] = m_world->getRoom(it.first)->getName();
    }
    std::string sOutput = "";

    if(m_sMode == "prosa")
        sOutput = m_gramma->build(func::to_vector(mapRooms), "Du warst schon in", "keinem anderen Raum.");
    else
        sOutput = "Räume in denen du schon warst: \n" + func::printList(mapRooms);
    appendDescPrint(sOutput + "\nGebe \"gehe [name des Raumes]\" ein, um direkt dort hinzugelangen.\n");
}


/**
* Look for shortes way (if exists) to given room. And return way as vector, or empty array.
* @param room players room
* @param roomID id of desired target-room
* @return vector with way to target.
*/
std::vector<std::string> CPlayer::findWay(CRoom* room, std::string roomID)
{
    //Check whether current room is target room
    if(room->getID() == roomID)
        return {};

    //Set variables
    std::queue<CRoom*> q;
    std::map<std::string, std::string> parents;

    //Set parent of every room to ""
    for(auto it : m_world->getRooms())
        parents[it.second->getID()] = "";

    //Normal breadth-first search
    q.push(room);
    parents[room->getID()] = room->getID();
    while(!q.empty())
    {
        CRoom* node = q.front(); 
        q.pop();
        if(node->getID() == roomID)
            break;
        for(auto& it : node->getExtits())
        {
            //Also check, that target-room is visited and in the same area as current room.
            if(parents[it.first] == "" && m_vistited[it.first] == true && node->getArea() == m_room->getArea())
            {
                q.push(m_world->getRoom(it.first));
                parents[it.first] = node->getID();
            }
        } 
    }

    //Check whether rooom has been found, if not, return empty array.
    if(parents[roomID] == "") 
        return {};

    //Create path from parents.  
    std::vector<std::string> path = { roomID };
    while(path.back() != room->getID())
        path.push_back(parents[path.back()]);
    path.pop_back();

    //Reverse path and return.
    std::reverse(std::begin(path), std::end(path));
    return path;
}


// *** Item and Inventory *** //

/**
* Adding all (non-hidden) items in room to players inventory.
*/
void CPlayer::addAll()
{
    std::vector<std::string> items_names;
    for(auto it = m_room->getItems().begin(); it != m_room->getItems().end();)
    {
        if((*it).second->getHidden() == false) 
	    {
            m_inventory.addItem((*it).second);
            items_names.push_back((*it).second->getName());
            m_room->getItems().erase((*(it++)).second->getID());
	        continue;
	    }
	    ++it;
    }

    appendDescPrint(m_gramma->build(items_names, "Du findest", "keine Gegenstände."));
}

/**
* Add given item to player's inventory and print message.
* @param item given item/ item to add to inventory.
*/
void CPlayer::addItem(CItem* item)
{
    m_inventory.addItem(item);
    appendDescPrint("<b> "+item->getName() + "</b> zu {name}'s Inventar hinzugefügt.\n");
    m_room->getItems().erase(item->getID());
}

/**
* Start a trade. Add a trade-context to player's context stack.
* @param partner Player's trading partner add to context-information.
*/
void CPlayer::startTrade(std::string partner)
{
    appendDescPrint("Started to trade with " + partner + ".\n");

    //Create trade-context and add to context-stack
    CEnhancedContext* context = new CEnhancedContext((std::string)"trade", {{"partner", partner}});
    context->print(this);
    m_contextStack.insert(context, 1, "trade");
}

/**
* Add all equipped items to player's output.
*/
void CPlayer::printEquiped() {

    auto getElem = [](CItem* item){ 
        std::string str; 
        if(item) str = item->getName(); 
        else str="nichts ausgerüstet.";
        return str;
    };
    appendPrint(func::table(m_equipment, getElem, "width:20%"));
}

/**
* Equipe given item into given category (sType). If there is already an item equipped in this 
* category, add choice-context to context-stack.
* @param item given item.
* @param sType type of item indicating category.
*/
void CPlayer::equipeItem(CItem* item, string sType)
{
    //If nothing is equipped in this category -> equip.
    if(m_equipment[sType] == NULL)
    {
        appendDescPrint("Du hast " + item->getName() + " als " + sType + " ausgerüstet.\n");
        string sAttack = item->getAttack();

        //Check for new attack
        if(sAttack != "") {
            m_attacks[sAttack] = m_world->getAttack(sAttack);
            appendDescPrint("Neue Attacke: \"" + m_attacks[sAttack]->getName() + "\".\n");
        }
        m_equipment[sType] = item;
    }

    //If this item is already equipped -> print error message.
    else if(m_equipment[sType]->getID() == item->getID())
        appendErrorPrint(sType + " bereits ausgerüstet.\n");

    //If another item is equipped in this category -> add choice-context
    else
    {
        appendErrorPrint("Bereits ein " + sType + " ausgerüstet. Austauschen? (ja/nein)\n");

        //Create Choice-Context
        m_contextStack.insert(new CEnhancedContext((nlohmann::json){{"name", "equipe"}, {"permeable", false},{"error", "Wähle ja oder nein\n"},{"itemID", item->getID()},{"handlers",{{"ja",{ "h_choose_equipe"}},{"nein",{"h_choose_equipe"}}}}}), 1, "equipe");
    }
}

/**
* Dequipe an item. Erase attack from list of attacks if it depended on equipped item.
* @param sType item type (weapon, clothing etc.)
*/
void CPlayer::dequipeItem(string sType) {
    if(m_equipment.count(sType) == 0)
        appendErrorPrint("Nothing to dequipe.\n");
    else if(m_equipment[sType] == NULL)
        appendErrorPrint("Nothing to dequipe.\n");
    else {
        appendDescPrint(sType + " " + m_equipment[sType]->getName() + " abgelegt.\n");

        //Erase attack
        if(m_equipment[sType]->getAttack() != "")
            m_attacks.erase(m_equipment[sType]->getAttack());
        m_equipment[sType] = NULL;
    }
}

// *** QUESTS *** //

/**
* show all active or solved quest depending on 'solved'
* @param solved indicating, whether to show solved or active quests.
*/
void CPlayer::showQuests(bool solved)
{

    if(solved == false)
        m_sPrint += "Active quests: \n";
    else
        m_sPrint += "Solved quests: \n";

    for(auto it : m_world->getQuests()) {
        if(it.second->getActive() == true)
            m_sPrint += "\n" + it.second->printQuest(solved);
    }
}

/**
* Add new quest by setting quest active.
* @param sQuestID id to given quest.
*/
void CPlayer::setNewQuest(std::string sQuestID)
{
    int ep=0;
    appendSuccPrint(m_world->getQuest(sQuestID)->setActive(ep));
    addEP(ep);
}

/**
* Set a quest-step as solved and add received ep (experience points) to players ep.
* @param sQuestID identifier to quest.
* @param sStepID identifier to quest-step.
*/
void CPlayer::questSolved(std::string sQuestID, std::string sStepID)
{
    int ep=0;
    appendSuccPrint(m_world->getQuest(sQuestID)->getSteps()[sStepID]->solved(ep));
    addEP(ep);
}


// *** Minds and Level *** //

/**
* Add experience points and call update-stats function if a new level is reached.
* @param ep experience points to be added.
*/
void CPlayer::addEP(int ep)
{
    m_ep+=ep;
    size_t counter=0;
    for(;m_ep>=20; m_ep-=20, counter++, m_level++)
        appendSuccPrint("Level Up!\n");

    if(counter > 0)
    {
        CEnhancedContext* context = m_contextStack.getContext("updateStats");
        if(context == NULL)
            updateStats(counter);
        else
        {
            context->setAttribute<int>("numPoints", context->getAttribute<int>("numPoints")+counter);
            m_sPrint+= "Du hast " + std::to_string(context->getAttribute<int>("numPoints")) + " Punkte zu vergeben!\n";
            for(size_t i=0; i<m_abbilities.size(); i++)
                m_sPrint += std::to_string(i+1) + ". " + m_abbilities[i] + ": level(" + std::to_string(getStat(m_abbilities[i])) + ")\n";
            m_sPrint += "Wähle eine Zahl geben den Namen des Attributes aus.\n";
        }
    }
}

/**
* Let player know how many learning points player can assign and add choice context.
* @param numPoints experience points player can assign.
*/
void CPlayer::updateStats(int numPoints)
{
    m_sPrint+= "Du hast " + std::to_string(numPoints) + " Punkte zu vergeben.\n";

    //Print attributes and level of each attribute and add choice-context.
    std::string sError = "Wähle eine Zahl oder den Namen des Attributes aus.\n";
    CEnhancedContext* context = new CEnhancedContext((nlohmann::json){{"name", "updateStats"}, {"permeable", false},{"numPoints", numPoints}, {"error", sError}});

    for(size_t i=0; i<m_abbilities.size(); i++)
        m_sPrint += std::to_string(i+1) + ". " + m_abbilities[i] + ": level(" + std::to_string(getStat(m_abbilities[i])) + ")\n";

    
    context->add_listener("h_updateStats", m_abbilities, 1);

    m_sPrint += sError;
    m_contextStack.insert(context, 1, "updateStats");
}

/**
* Print minds of player by using table function.
*/
void CPlayer::showMinds()
{
    m_sPrint += " --- " + m_sName + " --- \n"
            += "Level: " + std::to_string(m_level) + "\n"
            += "Ep: " + std::to_string(m_ep) + "/20.\n";

    auto lamda = [](SMind mind) { return std::to_string(mind.level);};
    m_sPrint += func::table(m_minds, lamda);
}

/**
* Print player's stats by using table function.
*/
void CPlayer::showStats() {

    m_sPrint += "--- " + m_sName + "---\n";
    
    std::map<std::string, std::map<std::string, std::string>> mapStats = m_world->getConfig()["mapAttributes"];
    std::map<std::string, int> mStats;
    for(auto it : m_stats)
    {
        if(mapStats.count(it.first) == 0)
            mStats[it.first] = it.second;
    }
    auto lambda = [](int stat) { return std::to_string(stat); };
    m_sPrint += func::table(mStats, lambda);
    for(auto it : m_stats)
    {
        if(mapStats.count(it.first) > 0)
            m_sPrint += mapStats[it.first][std::to_string(it.second)] + "\n";
    }
}

/**
* Check given dependencies. Receive a json and check whether this matches player's 
* minds or stats.
* @param jDeps json with dependencies
*/
bool CPlayer::checkDependencies(nlohmann::json jDeps)
{
    std::map<std::string , std::function<bool(int, int)>> operators;
    operators["="] = [](int a, int b) { return a=b; };
    operators[">"] = [](int a, int b) { return a>b; };
    operators[">="] = [](int a, int b) { return a>=b; };
    operators["<"] = [](int a, int b) { return a<b; };
    operators["<="] = [](int a, int b) { return a<=b; };
    operators["!="] = [](int a, int b) { return a!=b; };
    operators["&"] = [](int a, int b) { return true; };

    if(jDeps.size() == 0)
        return true;
    for(auto it=jDeps.begin(); it!=jDeps.end(); it++)
    {
        //Assign value with position 0 = operator
        std::string sOpt = func::extractLeadingChars(it.value());
        int value = func::getNumFromBack(it.value());

        if(sOpt == "&")
        {
            double level = m_minds[it.key()].level + m_minds[it.key()].relevance;
            for(const auto& mind : m_minds)
            {
                if(mind.first != it.key() && mind.second.level + mind.second.relevance > level)
                    return false;
            }
        }

        //Check dependency in mind
        if(m_minds.count(it.key()) > 0) {
            if(operators[sOpt](m_minds[it.key()].level, value) == false)
                return false;
        }

        //Check dependency in stats
        else if(m_stats.count(it.key()) > 0) {
            if(operators[sOpt](m_stats[it.key()], value) == false)
                return false;
        }
        else {
            std::cout << cRED << "Error in document: " << it.key() << cCLEAR << std::endl;
            return false;
        }
    } 

    return true;
}


// *** Others *** // 

/**
* Check if player's output contains special commands such as printing player name or else.
*/ 
void CPlayer::checkCommands()
{
    while(m_sPrint.find("{") != std::string::npos)
    {
        size_t pos  = m_sPrint.find("{");
        size_t pos2 = m_sPrint.find("}"); 
        std::string cmd = m_sPrint.substr(pos+1, pos2-(pos+1));
        std::string replace = "";
        if(cmd.find("name") != std::string::npos)
            replace = getName();
        m_sPrint = m_sPrint.substr(0, pos) + replace + m_sPrint.substr(pos2+1);
    }

    for(const auto& it : m_minds) 
    {
        size_t pos=0;
        for(;;)
        {
            if(m_sPrint.find(func::returnToUpper(it.first), pos) == std::string::npos)
                break;
            pos=m_sPrint.find(func::returnToUpper(it.first), pos);
            m_sPrint.insert(pos, it.second.color);
            m_sPrint.insert(pos + it.first.length()+1, WHITE);
            pos+=it.first.length();
        }
    }
}

/**
* Check whether given password and name matches this player.
* @param sName name to compare to player's name.
* @param sPassword password to compare to player's password.
*/
string CPlayer::doLogin(string sName, string sPassword)
{
    if(sName == func::returnToLower(m_sName) && sPassword == m_sPassword) 
        return m_sID;
    else 
        return "";
}

/**
* Get a player from currently online player's by their name using fuzzy comparison. 
* @param sIdentifier identifier (player's name)
*/
CPlayer* CPlayer::getPlayer(string sIdentifier)
{
    //Try to get player's id by using function from func.hpp
    auto lambda = [](CPlayer* player) { return player->getName(); };
    std::string sPlayerID = func::getObjectId(m_players, sIdentifier, lambda);

    //Check if player's id could be retrieved.
    if(sPlayerID != "")
        return m_players[sPlayerID];

    //If not, print error message on console and for player and return null-pointer.
    printError("FATAL ERROR!!! Player accessed that does not exist: " + sIdentifier);
    return nullptr;
}

/**
* Add a select-context to context-stack.
* @param mapObjects map of objects from which to select.
* @param sEventType type of event.
*/
void CPlayer::addSelectContest(std::map<std::string, std::string> mapObjects, std::string sEventType)
{
    //Add context.
    CEnhancedContext* context = new CEnhancedContext((nlohmann::json){{"name", "select"}, {"permeable",true},{"eventtype",sEventType}, {"map_objects",mapObjects}});

    //Set error function, to delete context when not called 
    context->setErrorFunction(&CEnhancedContext::error_delete); 

    //Add listeners/ eventhandlers
    context->add_listener("h_select", mapObjects, 1);
    
    //Insert context into context-stack.
    m_contextStack.insert(context, 1, "select");
}

/**
* Add chat context.
* @param sPlayer (chat partner (other player))
*/
void CPlayer::addChatContext(std::string sPartner)
{
    CEnhancedContext* context = new CEnhancedContext("chat", {{"partner", sPartner}});
    std::regex reg("(.*)");
    context->add_listener("h_send", reg, 1, 1);
    m_contextStack.insert(context, 1, "chat");
}

/**
* In the case of a serious error, leading game not not continue, let the player no
* that something went wrong and print error in console.
*/
void CPlayer::printError(std::string sError)
{
    std::cout << "\033[1;31m"+sError+"\033[0m"<< std::endl;
    appendTechPrint("Sorry, but something went wrong. Maybe try something else.\n");
}


// *** Eventmanager functions *** //

/**
* Throw event. This is the key function in the program. The parser first parses the command
* into an event which will then be thrown. An event consists of and event type 
* and an identifier indicating what will happen. For example "show people", where "show"
* is the event type and "people" tells the event handler what to do. (similar: "go to foyer").
* this event will be send through all contexts. Each context has a list of handlers. If the 
* event type matches with a handler a function is triggered. If a context is not permeable the 
* loop breaks.
* @param sInput
*/
void CPlayer::throw_events(string sInput, std::string sMessage)
{
    std::cout << cRED << "Events: " << sInput << ", from: " << sMessage << cCLEAR << std::endl;
    //Check for time triggered events
    checkTimeEvents();

    //Parse command
    std::vector<std::pair<std::string, std::string>> events = m_parser->parse(sInput);

    //Iterate over parsed events and call throw_event for each context and each event
    std::deque<CEnhancedContext*> sortedCtxList = m_contextStack.getSortedCtxList();
    for(size_t i=0; i<events.size(); i++)
    {
        std::cout << cRED << events[i].first << ", " << events[i].second << cCLEAR <<"\n";

        for(size_t j=0; j<sortedCtxList.size(); j++) {
            if(sortedCtxList[j]->throw_event(events[i], this) == false)
                break;
        }
    }
}


// ***** ***** TIME EVENTS ****** *****

/**
* Check if a time-bound event exists.
* @param sType event type.
*/
bool CPlayer::checkEventExists(string sType)
{
    return m_timeEventes.count(sType) > 0;
}

/**
* Add new time-bound event.
* @param sType event type 
* @param duration how long it takes till event will be triggered.
* @param func function called when event is triggered.
*/
void CPlayer::addTimeEvent(string sType, double duration, void (CPlayer::*func)(std::string), std::string sInfos)
{
    auto start = std::chrono::system_clock::now();
    m_timeEventes[sType].push_back(std::make_tuple(start, duration*60, func, sInfos));
}

/**
* check if a time event is triggered.
*/
void CPlayer::checkTimeEvents()
{
    std::list<std::pair<std::string, size_t>> lExecute;

    //Collect element to be executed
    auto end = std::chrono::system_clock::now();
    for(auto it : m_timeEventes)
    {
        size_t counter=0;
        for(auto jt : m_timeEventes[it.first]) {
            std::chrono::duration<double> diff = end - std::get<0>(jt);
            if(diff.count() >= std::get<1>(jt))
                lExecute.push_back(std::make_pair(it.first, counter));
        }
    }

    //Execute events and delete afterwards
    for(auto it : lExecute) 
    {
        std::tuple curT = m_timeEventes[it.first][it.second];
        (this->*std::get<2>(curT))(std::get<3>(curT));
        m_timeEventes[it.first].erase(m_timeEventes[it.first].begin() + it.second);
        if(m_timeEventes[it.first].size() == 0)
            m_timeEventes.erase(it.first);
    }
}

// *** Time handler *** //

/**
* Event triggered when highness decreases.
*/
void CPlayer::t_highness(std::string)
{
    if(m_stats["highness"]==0)
        return;
    appendStoryPrint("Time always comes to give you a hand; Things begin to become clearer again. Highness decreased by 1.\n");
    m_stats["highness"]--;

    if(m_stats["highness"]>0)
        addTimeEvent("highness", 2, &CPlayer::t_highness);
}

/**
* Event to throw any event after a certain time.
*/
void CPlayer::t_throwEvent(std::string sInfo)
{
    addPostEvent(sInfo);
}

