#ifndef CPLAYER_H
#define CPLAYER_H

#include <iostream>
#include <map>
#include <queue>
#include <stdlib.h>
#include <chrono> 
#include <time.h>
#include <ctime>
#include "CWorld.hpp"
#include "CPerson.hpp"
#include "SortedContext.hpp"
#include "CContext.hpp"
#include "CWorldContext.hpp"
#include "CStandardContext.hpp"
#include "CFightContext.hpp"
#include "CDialogContext.hpp"
#include "CChoiceContext.hpp"
#include "CChatContext.hpp"
#include "CRoom.hpp"
#include "CFight.hpp"
#include "func.hpp"

#include "JanGeschenk/Webconsole.hpp"
#include "JanGeschenk/Webgame.hpp"
 
using std::string;
using std::map;
using std::vector;

class CPlayer : public CPerson
{
private:
    CRoom* m_room;
    CWorld* m_world;

    size_t m_highness;

    string m_sPrint;
    string m_status;
    string m_sPassword;
    bool m_firstLogin;

    CFight* m_curFight;

    typedef map<string, vector<CItem*>> inventory;
    inventory m_inventory;
    typedef map<string, CItem*> equipment;
    equipment m_equipment;


    map<string, CPlayer*> m_players;

    CContextStack m_contextStack;
    Webconsole* _cout;

    typedef map<string, vector<std::tuple<std::chrono::system_clock::time_point, double, void(CPlayer::*)()>> > timeEvents;
    timeEvents m_timeEventes;

public:
    CPlayer() {};
    CPlayer(string sName,string password, string sID, int hp, size_t strength, int gold, CRoom* room, attacks newAttacks);


    // *** GETTER *** // 
    CRoom* getRoom();
    string getPrint();
    string getStatus();
    bool getFirstLogin();
    CFight* getFight();
    size_t getHighness();
    equipment& getEquipment();
    CWorld* getWorld();
    CContextStack& getContexts();

    // *** SETTER *** //
    void setRoom(CRoom* room);
    void setPrint(string);
    void appendPrint(string);
    void setStatus(string);
    void setFirstLogin(bool val);
    void setHighness(size_t highness);
    void setPlayers(map<string, CPlayer*> players);
    void setWobconsole(Webconsole*);

    //*** FUNCTIONS *** // 

    //Fight
    void setFight(CFight* fight);
    void endFight();

    //Dialog + Chat
    void startDialog(string sCharacter);
    void startChat(CPlayer* sPlayer);

    void send(string sMessage);

    //Login
    string doLogin(string sName, string sPassword);

    //Room
    void changeRoom(string sIdentifier);

    //Item and inventory
    void printInventory();
    void printEquiped();
    void addItem(CItem* item);
    void removeItem(string sItemName);
    CItem* getItem(string sName); 
    CItem* getItem_byID(string sID);
    void equipeItem(CItem*, string sType);
    void dequipeItem(string sType);

    //Stats
    string showStats();

    //Others
    void checkHighness();
    typedef std::map<string, string> objectmap;
    string getObject(objectmap& mapObjects, string sIdentifier);    
    CPlayer* getPlayer(string sIdentifier);


    void throw_event(string sInput);
    
    // *** Time events *** //

    void addTimeEvent(string sType, double duration, void(CPlayer::*func)());
    bool checkEventExists(string sType);
    void checkTimeEvents();

    // Time handler
    void t_highness();
};

#endif
    
