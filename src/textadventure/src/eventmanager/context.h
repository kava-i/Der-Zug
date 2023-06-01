#ifndef CENHANCEDCONTEXT_H 
#define CENHANCEDCONTEXT_H

#include <iostream>
#include <map>
#include <vector>

#include <nlohmann/json.hpp>

#include "concepts/quest.h"
#include "listener.h"
#include "objects/room.h"
#include "sorted_context.h"
#include "time_event.h"
#include "tools/func.h"
#include "tools/logic_parser.h"
#include "tools/webcmd.h"

class CPlayer;
class CGame;

class Context
{
protected:
    typedef std::map<std::string, std::string> map_type; 
    typedef std::pair<std::string, std::string> event;
    event m_curEvent;

    CContextStack<CListener> m_eventmanager;
    CContextStack<CTimeEvent> m_timeevents;
    
    nlohmann::json m_jAttributes;
    std::string m_sName;
    std::map<std::string, std::string> media_;
    bool m_permeable;
    bool m_curPermeable;
    bool m_block;
    std::string m_sHelp;
    std::string m_sError;

    CGame* m_game;

    void(Context::*m_error)(CPlayer*);

    static std::map<std::string, void(Context::*)(std::string&,CPlayer*)> listeners_;
    static std::map<std::string, nlohmann::json> m_templates;

public:

    Context(nlohmann::json jAttributes);
    Context(std::string sTemplate);
    Context(std::string sTemplate, nlohmann::json jAttributes);

    // *** GETTER *** //
    std::string getName();
    bool getPermeable();
    nlohmann::json getTemplate(std::string sTemplate);
    nlohmann::json& getAttributes();

    std::string GetFromMedia(std::string type);

    CContextStack<CTimeEvent> getTimeEvents() { return m_timeevents; }

    template<typename T> 
    T getAttribute(std::string attr) {
      if (m_jAttributes.count(attr) == 0) {
        std::cout << "Attributes requested that does not exist: " << attr << std::endl;
        return T();
      }
      return m_jAttributes[attr];
    }

    // *** SETTER *** //
    void setName(std::string sName);
    void setGame(CGame* game);
    void setErrorFunction(void(Context::*func)(CPlayer*));
    void setTimeEvents(CContextStack<CTimeEvent> stack) { m_timeevents=stack; } 
    void setCurPermeable(bool permeable);

    void SetMedia(std::string type, std::string filename);

    template<typename T> 
    T setAttribute(std::string sAttribute, T attribute)
    {
        if(m_jAttributes.count(sAttribute) == 0)
            std::cout << "About to set attribute that does not exist: " << sAttribute << std::endl;
        return m_jAttributes[sAttribute] = attribute;
    }

    // *** initializer *** //
    static void initializeHanlders();
    static void initializeTemplates();
    

    // *** Add and delete time events *** //
    void add_timeEvent(std::string id, std::string scope, std::string info, double duration, int priority=0);
    bool timeevent_exists(std::string type);
    void deleteTimeEvent(std::string);

    // *** Add and delete listeners *** //
    void AddSimpleListener(std::string handler, std::string event_type, int priority);
    void AddSimpleListener(std::string handler, std::regex event_type, int pos, int priority);
    void AddSimpleListener(std::string handler, std::vector<std::string> event_type, int priority);
    void AddSimpleListener(std::string handler, std::map<std::string, std::string> event_type, int priority);
    void AddListener(CListener* listener);

    // *** Throw events *** //
    bool throw_event(event newEvent, CPlayer* p);
    void throw_timeEvents(CPlayer* p);

    // *** ERROR HANDLER *** //
    void error(CPlayer* p);
    void error_delete(CPlayer* p);

    // *** BASIC HANDLER *** //
    void h_help(std::string&, CPlayer*);

    // *** GAME CONTEXT *** //
    void h_reloadGame(std::string&, CPlayer*);
    void h_reloadPlayer(std::string&, CPlayer*);
    void h_reloadWorlds(std::string&, CPlayer*);
    void h_reloadWorld(std::string&, CPlayer*);
    void h_updatePlayers(std::string&, CPlayer*);

    // *** WORLD CONTEXT *** //
    void h_printText(std::string&, CPlayer*);
    void h_finishCharacter(std::string&, CPlayer*);
    void h_killCharacter(std::string&, CPlayer*);
    void h_deleteCharacter(std::string&, CPlayer*);
    void h_addItem(std::string&, CPlayer*);
    void h_removeItem(std::string&, CPlayer*);
    void h_recieveMoney(std::string&, CPlayer*);
    void h_eraseMoney(std::string&, CPlayer*);
    void h_newFight(std::string&, CPlayer*);
    void h_endFight(std::string&, CPlayer*);
    void h_endDialog(std::string&, CPlayer*);
    void h_gameover(std::string&, CPlayer*);
    void h_addQuest(std::string&, CPlayer*);
    void h_showPersonInfo(std::string&, CPlayer*);
    void h_showItemInfo(std::string&, CPlayer*);
    void h_changeName(std::string&, CPlayer*);
    void h_addExit(std::string&, CPlayer*);
    void h_setAttribute(std::string&, CPlayer*);
    void h_setAttributeBound(std::string&, CPlayer*);
    void h_setMind(std::string&, CPlayer*);
    void h_setNewAttribute(std::string&, CPlayer*);
    void h_addTimeEvent(std::string&, CPlayer*);
    void h_setNewQuest(std::string&, CPlayer*);
    void h_changeDialog(std::string&, CPlayer*);
    void h_thieve(std::string&, CPlayer*);
    void h_attack(std::string&, CPlayer*);
    void h_changeRoomSilent(std::string&, CPlayer*);
    void h_changeRoom(std::string&, CPlayer*);
    void h_startDialogDirect(std::string&, CPlayer*);
    void h_startDialogDirectB(std::string&, CPlayer*);
    void h_changeSound(std::string&, CPlayer*);
    void h_changeImage(std::string&, CPlayer*);

    void h_moveCharToRoom(std::string&, CPlayer*);
    void h_moveDetailToRoom(std::string&, CPlayer*);
    void h_moveItemToRoom(std::string&, CPlayer*);

    void h_addCharToRoom(std::string&, CPlayer*);
    void h_addDetailToRoom(std::string&, CPlayer*);
    void h_addItemToRoom(std::string&, CPlayer*);

    void h_removeCharFromRoom(std::string&, CPlayer*);
    void h_removeDetailFromRoom(std::string&, CPlayer*);
    void h_removeItemFromRoom(std::string&, CPlayer*);

		std::pair<CRoom*, std::string> GetRoomAndObjectID(const std::string& sIdentifier, std::string& msg, 
				CPlayer* p); ///< helper function
		std::pair<CRoom*, CRoom*> GetRoomsAndObjectID(const std::string& sIdentifier, std::string& msg, 
				std::string& obj_id, CPlayer* p); ///< helper function

    void h_removeHandler(std::string&, CPlayer*);

    // *** STANDARD CONTEXT *** //
    void h_ignore(std::string&, CPlayer*);
    void h_show             (std::string&, CPlayer*);
    void h_look             (std::string&, CPlayer*);
    void h_search           (std::string&, CPlayer*);
    void h_take             (std::string&, CPlayer*);

    void h_consume          (std::string&, CPlayer*);
    void h_read             (std::string&, CPlayer*);
    void h_equipe           (std::string&, CPlayer*);
    void h_use 							(std::string&, CPlayer*);
    void UseItem (std::string& identifier, std::string category, CPlayer* p); ///< helper function

    void h_dequipe          (std::string&, CPlayer*);
    void h_examine          (std::string&, CPlayer*);
    void h_goTo             (std::string&, CPlayer*);
    void h_startDialog      (std::string&, CPlayer*);
    void h_try(std::string&, CPlayer*);                 ///< PRogrammer function for testing
    void h_test(std::string&, CPlayer*);

    //Rooms
    void h_moveToHospital    (std::string&, CPlayer*);
    void h_exitTrainstation  (std::string&, CPlayer*);


    // *** FIGHT CONTEXT *** //
    void h_fight(std::string&, CPlayer*);

    // *** DIALOG CONTEXT *** //
    void initializeDialogListeners(std::string newState, CPlayer* p);
    void h_call(std::string&, CPlayer*);

    // *** TRADECONTEXT *** //
    void print(CPlayer* p);
    void h_sell(std::string&, CPlayer*);
    void h_buy(std::string&, CPlayer*);
    void h_exit(std::string&, CPlayer*);

    // *** CHATCONTEXT *** //
    void h_send(std::string&, CPlayer*);
    void h_end(std::string&, CPlayer*);

    // *** READCONTEXT *** //
    void h_stop(std::string&, CPlayer*);
    void h_next(std::string&, CPlayer*);
    void h_prev(std::string&, CPlayer*);
    void h_mark(std::string&, CPlayer*);
    void h_underline(std::string&, CPlayer*);
    

    // *** QUESTS *** //
    void h_react(std::string&, CPlayer*);
    void h_reden(std::string&, CPlayer*);
    void h_geldauftreiben(std::string&, CPlayer*);

    // *** OTHER CONTEXTS *** //
    void h_select(std::string&, CPlayer*);
    void h_choose_equipe(std::string&, CPlayer*);
    void h_updateStats(std::string&, CPlayer*);
    void h_levelUp(std::string&, CPlayer*);

    // ----- ***** TIME EVENTS ***** ------ //
    void t_highness(std::string&, CPlayer*);
		void t_setAttribute(std::string& str, CPlayer* p);
    void t_throwEvent(std::string&, CPlayer*);
};
    
#endif 
