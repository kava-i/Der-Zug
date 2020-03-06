#include "CPlayer.hpp"

#define GREEN Webcmd::set_color(Webcmd::color::GREEN)
#define RED Webcmd::set_color(Webcmd::color::RED)
#define BLUE Webcmd::set_color(Webcmd::color::BLUE)
#define PURPLE Webcmd::set_color(Webcmd::color::PURPLE)
#define WHITE Webcmd::set_color(Webcmd::color::WHITE)

CPlayer::CPlayer(nlohmann::json jAtts, CRoom* room, attacks newAttacks)
{
    m_sName = jAtts["name"];
    m_sPassword = jAtts["password"];
    m_firstLogin = true;
    m_sID = jAtts["id"];

    //Stats
    m_stats["highness"] = 0;
    m_stats["hp"]       = jAtts.value("hp", 40);
    m_stats["gold"]     = jAtts.value("gold", 5);
    m_stats["strength"] = jAtts.value("strength", 8);
    m_stats["skill"]    = jAtts.value("skill", 8);
    
    m_abbilities = {"strength", "skill"};

    //Character and Level
    m_level = 0;
    m_ep = 0;
    m_minds["perzeption"] = {"perzeption", BLUE, 0};
    m_minds["peddler"] = {"peddler", RED, 0};
    m_minds["drama"]  = {"drama", RED, 0};
    m_minds["logik"]  = {"logik", RED, 0};

    m_room = room;
    m_attacks = newAttacks;

    m_equipment["weapon"] = NULL;
    m_equipment["armor"]  = NULL;
    
    //Initiazize world
    m_world = new CWorld(this);

    //Initialize all rooms as not visited
    for(const auto& it : m_world->getRooms())
        m_vistited[it.second->getID()] = false;
    m_vistited[m_room->getID()] = true;

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

bool CPlayer::getFirstLogin() { return m_firstLogin; };
CFight* CPlayer::getFight() { return m_curFight; };
CPlayer::minds& CPlayer::getMinds() { return m_minds; }
std::vector<std::string> CPlayer::getAbbilities() { return m_abbilities; }
CPlayer::equipment& CPlayer::getEquipment()  { return m_equipment; }
CWorld* CPlayer::getWorld() { return m_world; }
CContextStack& CPlayer::getContexts()   { return m_contextStack; }

// *** SETTER *** // 
void CPlayer::setRoom(CRoom* room)          { m_lastRoom = m_room; m_room = room; }
void CPlayer::setPrint(string newPrint)     { m_sPrint = newPrint; }
void CPlayer::appendPrint(string newPrint)  { m_sPrint.append(newPrint); }
void CPlayer::setFirstLogin(bool val)       { m_firstLogin = val; }
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
    m_contextStack.insert(new CDialogContext(this, sCharacter), 1, "dialog");
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
    std::cout << "Sending message... \n";
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

    if(room != "") {
        changeRoom(getWorld()->getRooms()[room]);
        return;
    }

    auto lamda = [](CRoom* room) { return room->getName(); };
    room = getObject2(m_world->getRooms(), sIdentifier, lamda);
    std::vector<std::string> path = findWay(m_room, room);

    if(path.size() > 0)
    {
        std::string events;
        for(const auto& it : path)
            events += "go " + it + ";";
        std::cout << "GENERATED EVENTS: " << events << std::endl;
        events.pop_back();
        throw_event(events);
    }
    
    else
        m_sPrint += "Room not found.\n";
}

void CPlayer::changeRoom(CRoom* newRoom)
{
    m_sPrint += newRoom->showEntryDescription(getWorld()->getCharacters());
    m_lastRoom = m_room; 
    m_room = newRoom;
    m_vistited[m_room->getID()] = true;
}

std::vector<std::string> CPlayer::findWay(CRoom* room, std::string roomID)
{
    if(room->getID() == roomID)
        return {};

    std::queue<CRoom*> q;
    std::map<std::string, std::string> parents;

    for(auto it : m_world->getRooms())
        parents[it.second->getID()] = "";

    std::cout << "Started searching.\n";
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
            if(parents[it.first] == "" && m_vistited[it.first] == true)
            {
                q.push(m_world->getRooms()[it.first]);
                parents[it.first] = node->getID();
            }
        } 
    }

    if(parents[roomID] == "") {
        std::cout << "Room not found.\n";
        return {};
    }
    else
        std::cout << "Room found... converting\n";

    std::vector<std::string> path = { roomID };
    while(path.back() != room->getID())
        path.push_back(parents[path.back()]);
    path.pop_back();
    std::reverse(std::begin(path), std::end(path));
    return path;
}


// *** Item and inventory *** //
void CPlayer::addAll()
{
    for(auto it = m_room->getItems().begin(); it != m_room->getItems().end();)
    {
        if((*it).second->getAttribute<bool>("hidden") == false) 
	{
            addItem((*(it++)).second);
	    continue;
	}
	++it;
    }
	
}

void CPlayer::addItem(CItem* item)
{
    m_inventory.addItem(item);
    m_sPrint += item->getName() + " added to " + m_sName + "'s inventory.\n";
    m_room->getItems().erase(item->getID());
}

void CPlayer::printEquiped() {

    auto getElem = [](CItem* item){ 
        std::string str; 
        if(item) str = item->getName(); 
        else str="empty handed as it seems.";
        return str;
    };
    m_sPrint += func::table(m_equipment, getElem, "width:20%");
}

void CPlayer::startTrade(std::string partner)
{
    m_sPrint += "Started to trade with " + partner + ".\n";

    m_contextStack.insert(new CTradeContext(this, partner), 1, "trade");
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
        updateStats(counter);
}

void CPlayer::updateStats(int numPoints)
{
    m_sPrint+= "Du hast " + std::to_string(numPoints) + " Punkte zu vergeben.\n";

    std::string sError = "Nummer, oder Attribut wählen.\n";
    CChoiceContext* context = new CChoiceContext(std::to_string(numPoints), sError);
    for(size_t i=0; i<m_abbilities.size(); i++)
    {
        m_sPrint += std::to_string(i+1) + ". " + m_abbilities[i] + ": level(" + std::to_string(getStat(m_abbilities[i])) + ")\n";
        context->add_listener(func::returnToLower(m_abbilities[i]), &CContext::h_updateStats);
        context->add_listener(std::to_string(i+1), &CContext::h_updateStats);
    }

    m_sPrint += sError;
    m_contextStack.insert(context, 1, "choice");
}

void CPlayer::showMinds()
{
    m_sPrint += " --- " + m_sName + " --- \n"
            += "Level: " + std::to_string(m_level) + "\n"
            += "Ep: " + std::to_string(m_ep) + "/20.\n";

    auto lamda = [](SMind mind) { return std::to_string(mind.level);};
    m_sPrint += func::table(m_minds, lamda);
}

void CPlayer::showStats() {

    m_sPrint += "Name: " + m_sName + "\n";
    auto getElem = [](int x){return std::to_string(x);};
    m_sPrint += func::table(m_stats, getElem);
}

bool CPlayer::checkDependencies(nlohmann::json jDeps)
{
    if(jDeps.size() == 0)
        return true;
    for(auto it=jDeps.begin(); it!=jDeps.end(); it++)
    {
        //Check dependecy in mind
        if(m_minds.count(it.key()) > 0) {
            int val = it.value();
            if(val < 0 && val*(-1) < m_minds[it.key()].level)
                return false;
            else if(val > 0 && val > m_minds[it.key()].level)
                return false;
        }

        //Check dependecy in stats
        else if(m_stats.count(it.key()) > 0) {
            int val = it.value();
            if(val < 0 && val*(-1) < m_stats[it.key()])
                return false;
            else if(val > 0 && val > m_stats[it.key()])
                return false;
        }
        else
            std::cout << "Error in document: " << it.key() << std::endl;
    } 
    return true;
}
    

// *** Others *** // 
void CPlayer::checkHighness()
{
    if(m_stats["highness"]==0)
        return; 

    srand(time(NULL));
    std::vector<string> words = func::split(m_sPrint, " ");

    size_t limit = (11-m_stats["highness"])/2;

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
    if(m_stats["highness"]==0)
        return;
    m_sPrint += "Time always comes to give you a hand; Things begin to become clearer again. Highness decreased by 1.\n";
    m_stats["highness"]--;

    if(m_stats["highness"]>0)
        addTimeEvent("highness", 2, &CPlayer::t_highness);
}

