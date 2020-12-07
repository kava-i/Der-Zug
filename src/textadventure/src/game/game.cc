#include "game.h" 
#include <chrono>
#include <thread>

// *** GETTER ***
map<string, CPlayer*> CGame::getPlayers() {
  return m_players;
}
map<string, nlohmann::json> CGame::getPlayerJsons() {
  return m_playerJsons;
}

CGame::CGame(std::string path) {
  std::cout << "Creating game.\n";

  Context::initializeHanlders();
  Context::initializeTemplates();
  m_context = new Context((std::string)"game");
  m_context->setGame(this);

  m_world = new CWorld(NULL, path);
  std::cout << "Done parsing world." << std::endl;
  //m_gramma = new CGramma({"dictionary.txt", "EIG.txt"});
  m_gramma = new CGramma({});

  //Create players
  playerFactory();
  std::cout << "Finished parsing!\n";
}
    

void CGame::playerFactory(bool update) {
  std::cout << "Parsing players... \n";

  //Read json creating all rooms
  std::ifstream read(m_world->getPathToWorld() + "players/players.json");
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

void CGame::playerFactory(nlohmann::json j_player) {
  //Create attacks
  map<string, CAttack*> attacks = m_world->parsePersonAttacks(j_player);
  if (m_world->getRooms().count(j_player["room"]) > 0) {
    m_players[j_player["id"]] = new CPlayer(j_player, 
      m_world->getRooms()[j_player["room"]], attacks, m_gramma, 
      m_world->getPathToWorld());
  }
  else 
    std::cout << "Room: " << j_player["room"] << " not found!" << std::endl;
}


string CGame::checkLogin(string sName, string sPassword, bool login, std::string& id) {
  std::cout << "checkLogin\n";
  
  if (login == true) {
    for(auto &it : m_players) {
      string tmp = it.second->doLogin(sName,sPassword);
      if(tmp != "") {
        id = tmp;
        return "Loggen in as " + id + "\n\n";
      }
    }
  } 
  return Webcmd::set_color(Webcmd::color::RED) + "Invalid login please try again!"
    + Webcmd::set_color(Webcmd::color::WHITE) + "\n\nName: ";
}

// ****************** FUNCTIONS CALLER ********************** //

string CGame::startGame(string sInput, string player_id, Webconsole* _cout) {
  if (m_players.count(player_id) > 0)
    m_players[player_id]->setWebconsole(_cout);
  else {
    std::cout << "Player " << player_id << " not found." << std::endl;
    return "";
  }
  if(m_players[player_id]->getFirstLogin() == true)
    m_players[player_id]->setFirstLogin(false);
  m_players[player_id]->throw_events("show room", "startGame");

  return m_players[player_id]->getPrint();
}

string CGame::play(string sInput, string sPlayerID, std::list<string>& 
    onlinePlayers) {
  func::convertToLower(sInput);    

  //Create map of online player's
  std::map<string, string> mapOnlinePlayers;
  std::map<string, CPlayer*> mapOnlinePlayers2;
  for(auto it : onlinePlayers) {
    if(it != m_players[sPlayerID]->getID() && m_players[sPlayerID]->getRoom()->getID() 
        == m_players[it]->getRoom()->getID()) {
      mapOnlinePlayers[m_players[it]->getID()] = m_players[it]->getName();
      mapOnlinePlayers2[m_players[it]->getID()] = m_players[it];
    }
  }

  //Create player
  m_curPlayer = m_players[sPlayerID];
  m_curPlayer->setPrint("");
  m_curPlayer->getRoom()->setPlayers(mapOnlinePlayers);
  m_curPlayer->setPlayers(mapOnlinePlayers2);

  if (sInput == "[end_game]" && m_curPlayer->getID() == "_admin") {
    std::cout << "Sending to all online players" << std::endl;
    for (auto it : onlinePlayers) {
      if (it == m_curPlayer->getID()) continue;
      m_players[it]->send("Game closed by host.\n");
    }
    std::cout << "Sending to host." << std::endl;
    m_curPlayer->send("Confirm closing...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return "[### end_game ###]";
  }

  //Check whether player is dead
  if(m_curPlayer->getStat("hp") <= 0) {
    m_context->throw_event(std::make_pair("reload_player", 
          m_curPlayer->getID()), m_players["programmer"]);
  }

  //Parse commands
  CParser parser(m_world->getConfig());
  std::vector<event> events = parser.parse(sInput);

  //Check for programmer commands
  if(m_curPlayer->getID() == "_admin") {
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
bool CGame::reloadPlayer(string sID) {
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

bool CGame::reloadWorld() {
  for(auto it : m_players)
    reloadWorld(it.first);
  return true;
}

bool CGame::reloadWorld(string sID) {
  if(m_players.count(sID) == 0)
    return false;

  //Set new world and reload current room of player
  m_players[sID]->setWorld(new CWorld(m_players[sID], 
        m_world->getPathToWorld()));

  m_players[sID]->setRoom(m_players[sID]->getWorld()->getRooms()[m_players[sID]
      ->getRoom()->getID()]);

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


