#ifndef CPLAYER_H
#define CPLAYER_H


#include <iostream>
#include <map>
#include <queue>
#include <stdlib.h>
#include <chrono> 
#include <time.h>
#include <ctime>
#include "CParser.hpp"
#include "CWorld.hpp"
#include "CPerson.hpp"
#include "SortedContext.hpp"
#include "CContext.hpp"
#include "CWorldContext.hpp"
#include "CStandardContext.hpp"
#include "CFightContext.hpp"
#include "CDialogContext.hpp"
#include "CTradeContext.hpp"
#include "CChoiceContext.hpp"
#include "CQuestContext.hpp"
#include "CChatContext.hpp"
#include "CRoom.hpp"
#include "CFight.hpp"
#include "CMind.hpp"
#include "func.hpp"


#include <JanGeschenk/Webconsole.hpp>
#include <JanGeschenk/Webgame.hpp>
 
using std::string;
using std::map;
using std::vector;

class CPlayer : public CPerson
{
private:
    CRoom* m_room;
    CRoom* m_lastRoom;
    CWorld* m_world;

    int m_ep;
    int m_level;

    string m_sPrint;
    string m_sPassword;
    bool m_firstLogin;

    CFight* m_curFight;

    typedef map<string, SMind> minds;
    typedef map<string, CItem*> equipment;
    minds m_minds;
    equipment m_equipment;
    
    std::vector<std::string> m_abbilities;

    map<string, CPlayer*> m_players;

    std::map<std::string, bool> m_vistited;

    CContextStack m_contextStack;
    Webconsole* _cout;

    typedef map<string, vector<std::tuple<std::chrono::system_clock::time_point, double, void(CPlayer::*)()>> > timeEvents;
    timeEvents m_timeEventes;

public:
    CPlayer() {};
    CPlayer(nlohmann::json jAttributes, CRoom* room, attacks newAttacks);

    // *** GETTER *** // 
    CRoom* getRoom();
    string getPrint();
    bool getFirstLogin();
    CFight* getFight();
    minds& getMinds();
    std::vector<std::string> getAbbilities();
    equipment& getEquipment();
    CWorld* getWorld();
    CContextStack& getContexts();

    // *** SETTER *** //
    void setRoom(CRoom* room);
    void setPrint(string);
    void appendPrint(string);
    void setFirstLogin(bool val);
    void setPlayers(map<string, CPlayer*> players);
    void setWobconsole(Webconsole*);
    void setWorld(CWorld* newWorld);

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
    void changeRoom(CRoom* newRoom);
    std::vector<std::string> findWay(CRoom* room, std::string sRoomdID);

    //Equiping items
    void printEquiped();
    void equipeItem(CItem*, string sType);
    void dequipeItem(string sType);

    void addAll();
    void addItem(CItem* item);

    void startTrade(std::string partner);

    //Quests
    void showQuests(bool solved);
    void setNewQuest(std::string sQuestID);
    void questSolved(std::string sQuestID, std::string sStepID);

    //Minds and level
    void addEP(int ep);
    void updateStats(int numPoints); 
    void showMinds();
    void showStats();
    bool checkDependencies(nlohmann::json);

    //Others
    void checkCommands();
    void checkHighness();
    typedef std::map<string, string> objectmap;
    string getObject(objectmap& mapObjects, string sIdentifier);    
    CPlayer* getPlayer(string sIdentifier);
    void addSelectContest(objectmap& mapObjects, std::string sEventType);

    template<typename T1, typename T2> 
    std::string getObject2(std::map<std::string, T1>& mapObjects, std::string sName, T2& lamda)
    {
        if(mapObjects.count(sName) > 0)
            return sName;

        std::vector<std::pair<std::string, double>> matches;
        
        for(auto[i, it] = std::tuple{1, mapObjects.begin()}; it!=mapObjects.end(); i++, it++) {
            if(std::isdigit(sName[0]) && (i==stoi(sName)))
                return it->first;
            double match = fuzzy::fuzzy_cmp(lamda(it->second), sName);
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

    typedef std::pair<std::string, std::string> event;
    void throw_event(string sInput);
    
    // *** Time events *** //

    void addTimeEvent(string sType, double duration, void(CPlayer::*func)());
    bool checkEventExists(string sType);
    void checkTimeEvents();

    // Time handler
    void t_highness();
};

#endif
    
