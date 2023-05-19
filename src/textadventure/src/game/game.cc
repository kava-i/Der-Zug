#include "game.h" 
#include "eventmanager/context.h"
#include "game/world.h"
#include "objects/object.h"
#include "tools/func.h"
#include "tools/webcmd.h"
#include <chrono>
#include <exception>
#include <thread>

// *** GETTER ***
map<string, CPlayer*> CGame::getPlayers() {
  return m_players;
}
map<string, nlohmann::json> CGame::getPlayerJsons() {
  return m_playerJsons;
}

std::string CGame::get_music() const {
  std::string music = "";
  try {
    music = m_world->media("music/background");
  }
  catch (std::exception& e) {
    std::cout << "Failed getting music: " << e.what() << std::endl;
  }
  std::cout << "GOT MUSIC: " << music << std::endl;
  return music;
}

std::string CGame::get_background_image() const {
  return m_world->media("image/background");
}

nlohmann::json CGame::get_appearance_config() const {
  return m_world->getConfig()["appearance"];
}

std::string CGame::path() const {
  return path_;
}

CGame::CGame(std::string path) : path_(path){
  std::cout << "Creating game." << std::endl;

  m_context = new Context((std::string)"game");
  m_context->setGame(this);
}

bool CGame::SetupGame() {
  try {
    std::cout << "Setting up game..." << std::endl;
    //Create world
    m_world = new CWorld(NULL, path_);
    std::cout << "Done parsing world." << std::endl;

    //Create players
    playerFactory();
    std::cout << "Finished parsing!" << std::endl;
    return true;
  }
  catch (std::exception& e) {
    std::cout << cRED << "Error creating game: " << e.what() << cCLEAR << std::endl;
    return false;
  }
}
    
void CGame::playerFactory(bool update) {
  std::cout << "Parsing players..." << std::endl;

  //Read json creating all rooms
  nlohmann::json j_players = func::LoadJsonFromDisc(m_world->getPathToWorld() + "players/players.json");

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
  map<string, CAttack*> attacks = m_world->parsePersonAttacks(j_player.value("attacks", 
        std::vector<std::string>()));
  if (m_world->getRooms().count(j_player["room"]) > 0) {
    m_players[j_player["id"]] = new CPlayer(j_player, m_world->getRooms()[j_player["room"]], 
        attacks, m_world->getPathToWorld());
  }
  else 
    std::cout << "Room: " << j_player["room"] << " not found!" << std::endl;
}


string CGame::checkLogin(string in_id, string password, bool login, std::string& id) {
  std::cout << "checkLogin\n";
  
  //Log player in
  if (login == true) {
    if (m_players.count(in_id) == 0) {
      return Webcmd::set_color(Webcmd::color::RED) + "username not found."
        + Webcmd::set_color(Webcmd::color::WHITE) + "\n\nid: ";
    }
    else {
      string tmp = m_players[in_id]->doLogin(in_id, password);
      if(tmp != "") {
        id = tmp;
        return "Logged in as " + id + "\n\n";
      }
    }
  } 

  //Register player
  else {
    if (m_players.count(in_id) > 0) {
      return Webcmd::set_color(Webcmd::color::RED) + "id already exists"
          + Webcmd::set_color(Webcmd::color::WHITE) + "\n\nid: ";
    }
    else {
      //Create new player
      nlohmann::json player;
      player["name"] = in_id;
      player["password"] = password;
      player["id"] = in_id;
      player["hp"] = 10;
      player["strength"] = 0;
      player["attacks"] = nlohmann::json::object();
      player["room"] = "trainstation_eingangshalle";
      
      //Edit json of all players and add new player
      nlohmann::json players;
      std::ifstream read(path_ + "/players/players.json");
      read >> players;
      read.close();
      players[in_id] = player;

      //Write player to disc.
      std::ofstream write(path_ + "/players/players.json");
      write << players;
      write.close();

      //Reload players
      playerFactory(false);
      id = in_id;

      return "Loggen in as " + in_id + "\n\n";
    }
  }

  return Webcmd::set_color(Webcmd::color::RED) + "Invalid login please try again!"
    + Webcmd::set_color(Webcmd::color::WHITE) + "\n\nid: ";
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
    if(it != m_players[sPlayerID]->id() && m_players[sPlayerID]->getRoom()->id() 
        == m_players[it]->getRoom()->id()) {
      mapOnlinePlayers[m_players[it]->id()] = m_players[it]->name();
      mapOnlinePlayers2[m_players[it]->id()] = m_players[it];
    }
  }

  //Create player
  m_curPlayer = m_players[sPlayerID];
  m_curPlayer->setPrint("");
  m_curPlayer->getRoom()->setPlayers(mapOnlinePlayers);
  m_curPlayer->setPlayers(mapOnlinePlayers2);

  if (sInput == "[end_game]" && m_curPlayer->id() == "_admin") {
    std::cout << "Sending to all online players" << std::endl;
    for (auto it : onlinePlayers) {
      if (it == m_curPlayer->id()) continue;
      m_players[it]->send("Game closed by host.\n");
    }
    std::cout << "Sending to host." << std::endl;
    m_curPlayer->send("Confirm closing...\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return "[### end_game ###]";
  }

  // Parse commands
  CParser parser(m_world->getConfig());
  std::vector<event> events = parser.parse(sInput);

  // Check for programmer commands
  if (m_curPlayer->id() == "_admin") {
    for(size_t i=0; i<events.size(); i++)
      m_context->throw_event(events[i], m_curPlayer);
  }
  if (m_context->getPermeable() == false)
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
      ->getRoom()->id()]);

  return true;
}
