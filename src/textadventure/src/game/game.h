#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <stdio.h>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <streambuf>
#include "eventmanager/context.h"
#include "world.h"
#include "objects/room.h"
#include "objects/detail.h"
#include "objects/person.h"
#include "objects/player.h"
#include "objects/attack.h"
#include "actions/fight.h"
#include "actions/dialog.h"
#include "tools/fuzzy.h"
#include "tools/func.h"
#include "tools/webcmd.h"

//#include <Python.h>

using std::string;
using std::map;

class Webconsole;

class CGame
{
private:

    CWorld* m_world;
    map<string, CPlayer*> m_players;
    map<string, nlohmann::json> m_playerJsons;
    CPlayer* m_curPlayer;
    const std::string path_;

    Context* m_context;

    typedef std::pair<string, string> event;
    typedef std::map<string, string> objectmap;

public: 
    CGame(std::string path);
   
    // *** GETTER *** //
    map<string, CPlayer*> getPlayers();
    map<string, nlohmann::json> getPlayerJsons();
    std::string get_music() const;
    std::string get_background_image() const;
    nlohmann::json get_appearance_config() const;
    std::string path() const;

    // *** FACTORYS *** // 
    bool SetupGame();
    void playerFactory(bool update=false);
    void playerFactory(nlohmann::json j_player);

    // *** INPUT HANDLERS *** //
    string startGame(string sInput, string sPasswordID, Webconsole* _cout);
    string play(string sInput, string sPlayerID, std::list<string>& onlinePlayers);

    // *** FUNCTIONS *** //
    string checkLogin(string sName, string sPassword, bool login, std::string& id);  
    bool reloadPlayer(string sPlayerID);
    bool reloadWorld();
    bool reloadWorld(string sPlayerID);
};
    





