#include "CGame.hpp" 

// *** GETTER ***
map<string, CPlayer*> CGame::getPlayers() {
    return m_players;
}
map<string, nlohmann::json> CGame::getPlayerJsons() {
    return m_playerJsons;
}

CGame::CGame() {

    std::cout << "Creating game.\n";

    CEnhancedContext::initializeHanlders();
    CEnhancedContext::initializeTemplates();
    m_context = new CEnhancedContext((std::string)"game");
    m_context->setGame(this);

    m_world = new CWorld(NULL);
    //m_gramma = new CGramma({"dictionary.txt", "EIG.txt"});
    m_gramma = new CGramma({});

    //Create players
    playerFactory();
    std::cout << "Finished parsing!\n";
}
    

void CGame::playerFactory(bool update)
{
    std::cout << "Parsing players... \n";

    //Read json creating all rooms
    std::ifstream read(m_world->getPathToWorld() + "jsons/players/players.json");
    nlohmann::json j_players;
    read >> j_players;
    read.close();

    for(auto j_player : j_players) {
        //Add json to list of player jsons
        m_playerJsons[j_player["id"]] = j_player;

        //Create player (when only updating, skip, when player already exists)
        if(update == false || m_players.count(j_player["id"]) == 0)
            playerFactory(j_player);
    }
}

void CGame::playerFactory(nlohmann::json j_player)
{
    //Create attacks
    map<string, CAttack*> attacks = m_world->parsePersonAttacks(j_player);
    m_players[j_player["id"]] = new CPlayer(j_player, m_world->getRooms()[j_player["room"]], attacks, m_gramma);
}


string CGame::checkLogin(string sName, string sPassword)
{
    std::cout << "checkLogin\n";
    for(auto &it : m_players) {
	    string tmp = it.second->doLogin(sName,sPassword);
	    if(tmp != "")
	        return tmp;
    }
    return "";
}

// ****************** FUNCTIONS CALLER ********************** //

string CGame::startGame(string sInput, string sPasswordID, Webconsole* _cout)
{
    m_players[sPasswordID]->setWobconsole(_cout);
    if(m_players[sPasswordID]->getFirstLogin() == true)
        m_players[sPasswordID]->setFirstLogin(false);
    m_players[sPasswordID]->throw_events("show room", "startGame");

    return m_players[sPasswordID]->getPrint();
}

string CGame::play(string sInput, string sPlayerID, std::list<string>& onlinePlayers)
{
    func::convertToLower(sInput);    

    //Create map of online player's
    std::map<string, string> mapOnlinePlayers;
    std::map<string, CPlayer*> mapOnlinePlayers2;
    for(auto it : onlinePlayers) {
        if(it != m_players[sPlayerID]->getID() && m_players[sPlayerID]->getRoom()->getID() == m_players[it]->getRoom()->getID()) {
            mapOnlinePlayers[m_players[it]->getID()] = m_players[it]->getName();
            mapOnlinePlayers2[m_players[it]->getID()] = m_players[it];
        }
    }

    //Create player
    m_curPlayer = m_players[sPlayerID];
    m_curPlayer->setPrint("");
    m_curPlayer->getRoom()->setPlayers(mapOnlinePlayers);
    m_curPlayer->setPlayers(mapOnlinePlayers2);

    //Check whether player is dead
    if(m_curPlayer->getStat("hp") <= 0) {
        m_context->throw_event(std::make_pair("reload_player", m_curPlayer->getID()), m_players["programmer"]);
    }

    //Parse commands
    CParser parser(m_world->getConfig());
    std::vector<event> events = parser.parse(sInput);

    //Check for programmer commands
    if(m_curPlayer->getID().find("programmer") != std::string::npos) {
        for(size_t i=0; i<events.size(); i++)
            m_context->throw_event(events[i], m_curPlayer);
    }
    if(m_context->getPermeable() == false)
        return m_curPlayer->getPrint();

    //Throw event of player
    m_curPlayer->throw_events(sInput, "CGame::play");
    
    return m_curPlayer->getPrint(); 
}

// ***** FUNCTIONS ***** //
bool CGame::reloadPlayer(string sID)
{
    if(m_players.count(sID) == 0)
        return false;

    //Delete old player
    delete m_players[sID];
    m_players.erase(sID);

    //Create new player
    playerFactory(m_playerJsons[sID]);
    m_curPlayer = m_players[sID];

    return true;
}

bool CGame::reloadWorld()
{
    for(auto it : m_players)
        reloadWorld(it.first);
    return true;
}

bool CGame::reloadWorld(string sID)
{
    if(m_players.count(sID) == 0)
        return false;

    //Set new world
    m_players[sID]->setWorld(new CWorld(m_players[sID]));

    //Reload current room of player
    m_players[sID]->setRoom(m_players[sID]->getWorld()->getRooms()[m_players[sID]->getRoom()->getID()]);

    return true;
}


/*
void CGame::toJson(string filename)
{
    //Convert yaml to json using python
	Py_Initialize();
	FILE* fp = _Py_fopen(filename, "r");
	PyRun_SimpleFile(fp, filename);

	Py_Finalize();
}
*/ 


