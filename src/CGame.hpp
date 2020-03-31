#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <stdio.h>
#include <experimental/filesystem>
#include <streambuf>
#include "CGameContext.hpp"
#include "CWorld.hpp"
#include "CRoom.hpp"
#include "CDetail.hpp"
#include "CPerson.hpp"
#include "CPlayer.hpp"
#include "CAttack.hpp"
#include "CFight.hpp"
#include "CDialog.hpp"
#include "json.hpp"
#include "fuzzy.hpp"
#include "func.hpp"

//#include <Python.h>

using std::string;
using std::map;
using std::vector;

class Webconsole;

class CGame
{
private:

    CWorld* m_world;
    map<string, CPlayer*> m_players;
    map<string, nlohmann::json> m_playerJsons;
    CPlayer* m_curPlayer;

    CGameContext* m_context;

    typedef std::pair<string, string> event;
    typedef std::map<string, string> objectmap;

public: 
    CGame();
   
    // *** GETTER *** //
    map<string, CPlayer*> getPlayers();
    map<string, nlohmann::json> getPlayerJsons();

    // *** FACTORYS *** // 
    void playerFactory(bool update=false);
    void playerFactory(nlohmann::json j_player);

    // *** INPUT HANDLERS *** //
    string startGame(string sInput, string sPasswordID, Webconsole* _cout);
    string play(string sInput, string sPlayerID, std::list<string>& onlinePlayers);

    // *** FUNCTIONS *** //
    string checkLogin(string sName, string sPassword);  
    bool reloadPlayer(string sPlayerID);
    bool reloadWorld();
    bool reloadWorld(string sPlayerID);

    /*
    //Convert yaml to json
    void toJson(string filename);
    */
};
    





