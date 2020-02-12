#include "CPlayer.hpp"

#define GREEN Webcmd::set_color(Webcmd::color::GREEN)
#define WHITE Webcmd::set_color(Webcmd::color::WHITE)

CPlayer::CPlayer(string sName,string sPassword, string sID, int hp, size_t strength, int gold, CRoom* room, attacks newAttacks)
{
    m_sName = sName;
    m_sPassword = sPassword;
    m_firstLogin = true;
    m_sID = sID;
    m_hp = hp;
    m_gold = gold;
    m_strength = strength;
    m_highness = 0;

    //Character and Level
    m_level = 0;
    m_ep = 0;
    m_minds["Die Perzeption"] = 0;
    m_minds["Der Krämer"] = 0;
    m_minds["Die Kraft"]  = 0;
    m_minds["Die Logik"]  = 0;

    m_room = room;
    m_status = "standard";
    m_attacks = newAttacks;

    m_equipment["weapon"] = NULL;
    m_equipment["armor"]  = NULL;
    
    //Initiazize world
    m_world = new CWorld();

    for(auto it : m_world->getQuests())
        m_contextStack.insert(new CQuestContext(it.second), 3, it.first);

    //Add eventhandler to eventmanager
    m_contextStack.insert(new CWorldContext(), 2, "world");
    m_contextStack.insert(new CStandardContext(), 0, "standard");
}

// *** GETTER *** // 
CRoom* CPlayer::getRoom()   { return m_room; }

string CPlayer::getPrint()  { 
    checkHighness();
    m_sPrint+="\n\n";
    return m_sPrint; 
}

string CPlayer::getStatus() { return m_status; };
bool CPlayer::getFirstLogin() { return m_firstLogin; };
CFight* CPlayer::getFight() { return m_curFight; };
size_t CPlayer::getHighness() { return m_highness; };
CPlayer::minds& CPlayer::getMinds() { return m_minds; }
CPlayer::equipment& CPlayer::getEquipment()  { return m_equipment; }
CWorld* CPlayer::getWorld() { return m_world; }
CContextStack& CPlayer::getContexts()   { return m_contextStack; }

// *** SETTER *** // 
void CPlayer::setRoom(CRoom* room)          { m_lastRoom = m_room; m_room = room; }
void CPlayer::setPrint(string newPrint)     { m_sPrint = newPrint; }
void CPlayer::appendPrint(string newPrint)  { m_sPrint.append(newPrint); }
void CPlayer::setStatus(string status)      { m_status = status; }
void CPlayer::setFirstLogin(bool val)       { m_firstLogin = val; }
void CPlayer::setHighness(size_t highness)  { m_highness = highness; }
void CPlayer::setPlayers(map<string, CPlayer*> players) { m_players = players; }
void CPlayer::setWobconsole(Webconsole* webconsole) { _cout = webconsole; }
void CPlayer::setWorld(CWorld* newWorld)    { m_world = newWorld; }



// *** *** FUNCTIONS *** *** // 


// *** Fight *** //
void CPlayer::setFight(CFight* newFight) { 
    m_curFight = newFight;
    m_contextStack.insert(new CFightContext(m_attacks), 1, "fight");
    m_curFight->initializeFight();
}

void CPlayer::endFight() {
    delete m_curFight;
    m_contextStack.erase("fight");
    m_sPrint += "Fight ended.\n";
}

// *** Dialog *** //
void CPlayer::startDialog(string sCharacter)
{
    m_dialog = m_world->getCharacters()[sCharacter]->getDialog();
    m_contextStack.insert(new CDialogContext(this), 1, "dialog");
    throw_event(m_dialog->states["START"]->callState(this));
}

void CPlayer::startChat(CPlayer* player)
{
    m_sPrint += "Du gehst auf " + player->getName() + " zu und räusperst dich... \n";
    if(player->getContexts().nonPermeableContextInList() == true)
        m_sPrint += "\n" + player->getName() + " ist zur Zeit beschäftigt.\n"; 
    else
    {
        //Add Chat context for both players
        m_contextStack.insert(new CChatContext(player), 1, "chat");
        player->getContexts().insert(new CChatContext(this), 1, "chat");
        
        //Send text by throwing event
        throw_event("Hey " + player->getName() + ".");
    }
}

void CPlayer::send(string sMessage)
{
    _cout->write(sMessage);
    _cout->flush(); 
}

// *** Room *** 
void CPlayer::changeRoom(string sIdentifier)
{
    //Check if player wants to go back
    if(sIdentifier == "back") {
        changeRoom(m_lastRoom);
        return;
    }

    //Get selected room
    string room = getObject(getRoom()->getExtits(), sIdentifier);

    //Check if room was found
    if(room == "") {
        m_sPrint += "Room/ exit not found\n";
        return;
    }

    //Print description and change players current room
    changeRoom(getWorld()->getRooms()[room]);
}

void CPlayer::changeRoom(CRoom* newRoom)
{
    m_sPrint += newRoom->showEntryDescription(getWorld()->getCharacters());
    m_lastRoom = m_room; 
    m_room = newRoom;
}


// *** Item and inventory *** //
void CPlayer::printInventory() {
    m_sPrint += m_sName + "'s Inventory: \n";

    string sEquipment = "Equipment: ";
    string sConsume = "Food and Drinks: ";
    string sOthers = "Others: ";
    for(auto it : m_inventory) {
        if(it.second[0]->getAttribute<string>("type").find("equipe") != string::npos)
            sEquipment += std::to_string(it.second.size()) + "x " + it.second[0]->getName() + ", ";

        else if(it.second[0]->getAttribute<string>("type").find("consume") != string::npos)
            sConsume += std::to_string(it.second.size()) + "x " + it.second[0]->getName() + ", ";

        else 
            sOthers += std::to_string(it.second.size()) + "x " + it.second[0]->getName() + ", ";
    }
    m_sPrint += sEquipment +"\n"+ sConsume +"\n" + sOthers + "\n";
}


void CPlayer::printEquiped() {
    for(auto it : m_equipment) {
        if(it.second != NULL)
            m_sPrint += it.first + ": " + it.second->getName() + "\n"; 
        else
            m_sPrint += it.first + ": empty handed as it seems.\n";
    }
}

void CPlayer::addItem(CItem* item) {
    m_inventory[item->getAttribute<string>("name")].push_back(item);
    m_sPrint += item->getAttribute<string>("name") + " added to " + m_sName + "'s inventory.\n";
    m_room->getItems().erase(item->getAttribute<string>("id"));
}

void CPlayer::removeItem(string sItemName) {
    m_inventory[sItemName].pop_back();
    if(m_inventory[sItemName].size() == 0)
        m_inventory.erase(sItemName);
}

CItem* CPlayer::getItem(string sName)
{
    for(auto it : m_inventory) {
        if(fuzzy::fuzzy_cmp(it.second[0]->getName(), sName) <= 0.2) 
            return it.second[0];
    }
    m_sPrint += "Item not in inventory! (use \"show inventory\" to see your items.)\n";
    return NULL;
}

CItem* CPlayer::getItem_byID(string id)
{
    for(auto it : m_inventory) {
        if(it.second[0]->getID() == id)
            return it.second[0];
    }
    return NULL;
}

void CPlayer::equipeItem(CItem* item, string sType)
{
    if(m_equipment[sType] == NULL)
    {
        m_sPrint += "You equiped " + sType + ": " + item->getName() + ".\n";
        string sAttack = item->getAttribute<string>("attack");
        if(sAttack != "") {
            m_attacks[sAttack] = m_world->getAttacks()[sAttack];
            m_sPrint += "New attack: \"" + m_attacks[sAttack]->getName() + "\" added to arracks.\n";
        }
        m_equipment[sType] = item;
    }

    else if(m_equipment[sType]->getID() == item->getID())
        m_sPrint+=sType + " already equiped.\n";
    else
    {
        m_sPrint+="Already a " + sType + " equipt. Want to change? (yes/no)\n";

        //Create Choice-Context
        CChoiceContext* context = new CChoiceContext(item->getID(), "Choose only yes or no\n");
        context->add_listener("yes", &CContext::h_choose_equipe);
        context->add_listener("no", &CContext::h_choose_equipe);
        m_contextStack.insert(context, 1, "choice");
    }
}

void CPlayer::dequipeItem(string sType) {
    if(m_equipment.count(sType) == 0)
        m_sPrint += "Nothing to dequipe.\n";
    else if(m_equipment[sType] == NULL)
        m_sPrint += "Nothing to dequipe.\n";
    else {
        m_sPrint += "Dequiped " + sType + " " + m_equipment[sType]->getName() + ".\n";
        string sAttack = m_equipment[sType]->getAttribute<string>("attack");
        if(sAttack != "")
            m_attacks.erase(sAttack);
        m_equipment[sType] = NULL;
    }
}

// *** QUESTS *** //

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

void CPlayer::setNewQuest(std::string sQuestID)
{
    std::cout << sQuestID << std::endl;
    int ep=0;
    m_sPrint += m_world->getQuests()[sQuestID]->setActive(ep);
    addEP(ep);
}

void CPlayer::questSolved(std::string sQuestID, std::string sStepID)
{
    int ep=0;
    m_sPrint+=m_world->getQuests()[sQuestID]->getSteps()[sStepID]->solved(ep);
    addEP(ep);
}

//Minds and Level
void CPlayer::addEP(int ep)
{
    m_ep+=ep;
    size_t counter=0;
    for(;m_ep>=20; m_ep-=20, counter++){
        m_level++;
        m_sPrint+= GREEN + "Level Up!" + WHITE + "\n";
    }
    if(counter > 0)
        updateMinds(counter);
}

void CPlayer::updateMinds(int numPoints)
{
    m_sPrint+= "Du hast " + std::to_string(numPoints) + " Punkte zu vergeben.\n";
    std::string sError = "Nummer auswählen, oder \"mind\" (ohne Artikel!)\n";
    CChoiceContext* context = new CChoiceContext(std::to_string(numPoints), sError);
    for(auto it : m_minds){
        m_sPrint += it.first + ": level(" + std::to_string(it.second) + ")\n";
        context->add_listener(func::returnToLower(it.first).substr(4), &CContext::h_updateMind);
    }
    m_sPrint += sError;
    m_contextStack.insert(context, 1, "choice");
}

void CPlayer::showMinds()
{
    m_sPrint += " --- " + m_sName + " --- \n"
            += "Level: " + std::to_string(m_level) + "\n"
            += "Ep: " + std::to_string(m_ep) + "/20.\n";
    for(auto it : m_minds)
        m_sPrint += it.first + ": level(" + std::to_string(it.second) + ")\n";
    m_sPrint+="\n";
}
    

// *** Stats *** //
string CPlayer::showStats() {
    string stats = "Name: " + m_sName 
        + "\nHP: " + std::to_string(m_hp) 
        + "\nStrength: " + std::to_string(m_strength)
        + "\nHighness: " + std::to_string(m_highness) 
        + "\n" 
        + printAttacks();

    return stats;
}


// *** Others *** // 
void CPlayer::checkHighness()
{
    if(m_highness==0)
        return; 

    srand(time(NULL));
    std::vector<string> words = func::split(m_sPrint, " ");

    size_t limit = (11-m_highness)/2;

    size_t counter = 0;
    for(auto& word : words)
    {
        if(counter%(limit)!= 0) {
            counter++;
            continue;
        }

        for(size_t i=0; i<word.size(); i++) {

            if(i%(limit) != 0 || isalpha(word[i]) == false)
                continue;
            size_t num = rand() % word.size()-1;
            if(!isalpha(word[num]))
                continue;

            char x = word[i];
            word[i] = word[num];
            word[num] = x;
        }
        counter++;
    }

    m_sPrint="";
    for(auto word : words)
        m_sPrint+=word + " ";
}


string CPlayer::doLogin(string sName, string sPassword)
{
    if(sName == m_sName && sPassword == m_sPassword) return m_sID;
    else return "";
}

string CPlayer::getObject(objectmap& mapObjects, string sIdentifier)
{
    if(mapObjects.count(sIdentifier) > 0)
        return sIdentifier;

    std::vector<std::pair<std::string, double>> matches;
    
    for(auto[i, it] = std::tuple{1, mapObjects.begin()}; it!=mapObjects.end(); i++, it++) {
        if(std::isdigit(sIdentifier[0]) && (i==stoi(sIdentifier)))
            return it->first;
        double match = fuzzy::fuzzy_cmp(it->second, sIdentifier);
        if(match <= 0.2) 
            matches.push_back(std::make_pair(it->first, match));
    }

    if(matches.size() == 0)
        return "";

    //Find best match.
    size_t pos=0;
    for(auto[i, max] = std::tuple{size_t{0}, 0.3}; i<matches.size(); i++) {
        if(matches[i].second < max) {
            pos=i;
            max=matches[i].second;
        }
    }
    return matches[pos].first;
}

CPlayer* CPlayer::getPlayer(string sIdentifier)
{
    for(auto it : m_players)
    {
        if(fuzzy::fuzzy_cmp(it.second->getName(), sIdentifier) <= 0.2)
            return it.second;
    }
    return NULL;
}

void CPlayer::addSelectContest(objectmap& mapObjects, std::string sEventType)
{
    CContext* context = new CChoiceContext(sEventType, mapObjects);
    
    for(size_t i=1; i<=mapObjects.size();i++)
        context->add_listener(std::to_string(i), &CContext::h_select);

    m_contextStack.insert(context, 1, "choice");
}


// ***** ***** EVENTMANAGER FUNCTIONS ***** *****

void CPlayer::throw_event(string sInput)
{
    checkTimeEvents();
    CParser parser;
    std::vector<event> events = parser.parse(sInput);
    std::deque<CContext*> sortedCtxList= m_contextStack.getSortedCtxList();

    for(size_t i=0; i<events.size(); i++)
    {
        std::cout << events[i].first << ", " << events[i].second << "\n";

        for(size_t j=0; j<sortedCtxList.size(); j++) {
            sortedCtxList[j]->throw_event(events[i], this);
            if(sortedCtxList[j]->getPermeable() == false)
                break;
        }
    }
}


// ***** ***** TIME EVENTS ****** *****
bool CPlayer::checkEventExists(string sType)
{
    return m_timeEventes.count(sType) > 0;
}
void CPlayer::addTimeEvent(string sType, double duration, void (CPlayer::*func)())
{
    auto start = std::chrono::system_clock::now();
    m_timeEventes[sType].push_back(std::make_tuple(start, duration*60, func));
}

void CPlayer::checkTimeEvents()
{
    std::list<std::pair<std::string, size_t>> lExecute;

    //Collect element to be executed
    auto end = std::chrono::system_clock::now();
    for(auto it : m_timeEventes)
    {
        std::cout << "ititializing... \n";
        size_t counter=0;
        for(auto jt : m_timeEventes[it.first]) {
            std::chrono::duration<double> diff = end - std::get<0>(jt);
            if(diff.count() >= std::get<1>(jt))
                lExecute.push_back(std::make_pair(it.first, counter));
        }
    }

    std::cout << "initializes execute. \n";

    //Execute events and delete afterwards
    for(auto it : lExecute) {
        std::cout << "Calling and deleting: " << it.first << "/" << it.second << std::endl;
        (this->*std::get<2>(m_timeEventes[it.first][it.second]))();
        m_timeEventes[it.first].erase(m_timeEventes[it.first].begin() + it.second);
        if(m_timeEventes[it.first].size() == 0)
            m_timeEventes.erase(it.first);
    }

}

// Time handler
void CPlayer::t_highness()
{
    if(m_highness==0)
        return;
    m_sPrint += "Time always comes to give you a hand; Things begin to become clearer again. Highness decreased by 1.\n";
    m_highness--;

    if(m_highness>0)
        addTimeEvent("highness", 0.25, &CPlayer::t_highness);
}

