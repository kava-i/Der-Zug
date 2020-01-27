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
#include "CCharacter.hpp"
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

    map<string, CPlayer*> m_players;
    CPlayer* m_curPlayer;
    CWorld* m_world;

    CGameContext* m_context;

    typedef std::pair<string, string> event;
    typedef std::map<string, string> objectmap;

public: 
    CGame();
   
    // *** FACTORYS *** // 
    void playerFactory();

    string startGame(string sInput, string sPasswordID, Webconsole* _cout);
    string play(string sInput, string sPlayerID, std::list<string>& onlinePlayers);

    string checkLogin(string sName, string sPassword);

    /*
    //Convert yaml to json
    void toJson(string filename);
    */
};
    





