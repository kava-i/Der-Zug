#include "CGame.hpp" 

CGame::CGame() {
    m_world = new CWorld();
    
    m_context = new CGameContext();
    m_context->setGame(this);

    //Create players
    playerFactory();
    std::cout << "Finished parsing!\n";
}
    

void CGame::playerFactory()
{
    std::cout << "Parsing players... \n";

    //Read json creating all rooms
    std::ifstream read("factory/jsons/players/players.json");
    nlohmann::json j_players;
    read >> j_players;
    read.close();

    for(auto j_player : j_players) {

        //Create attacks
        map<string, CAttack*> attacks = m_world->parsePersonAttacks(j_player);

        //Create player
        m_players[j_player["id"]] = new CPlayer(j_player["name"], j_player["password"],j_player["id"], j_player.value("hp", 40), j_player.value("strength", 8), j_player.value("gold", 5), m_world->getRooms()[j_player["room"]], attacks);
    }
}


string CGame::checkLogin(string sName, string sPassword)
{
    for(auto &it : m_players)
    {
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
    if(m_players[sPasswordID]->getFirstLogin() == true) {
        m_players[sPasswordID]->throw_event("startTutorial");
        m_players[sPasswordID]->setFirstLogin(false);
    }
    else
        m_players[sPasswordID]->throw_event("show room");

    return m_players[sPasswordID]->getPrint();
}

string CGame::play(string sInput, string sPlayerID, std::list<string>& onlinePlayers)
{
    //Check for programmer commands
    m_context->throw_event(sInput, NULL);
    if(m_context->getPermeable() == false)
        return "";

    func::convertToLower(sInput);    

    std::map<string, string> mapOnlinePlayers;
    std::map<string, CPlayer*> mapOnlinePlayers2;
    for(auto it : onlinePlayers)
    {
        if(it != m_players[sPlayerID]->getID())
        {
            mapOnlinePlayers[m_players[it]->getName()] = m_players[it]->getRoom()->getID();
            mapOnlinePlayers2[m_players[it]->getID()] = m_players[it];
        }
    }

    //Create player
    m_curPlayer = m_players[sPlayerID];
    m_curPlayer->setPrint("");
    m_curPlayer->getRoom()->setPlayers(mapOnlinePlayers);
    m_curPlayer->setPlayers(mapOnlinePlayers2);

    //Throw event 
    m_curPlayer->throw_event(sInput);

    return m_curPlayer->getPrint(); 
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


