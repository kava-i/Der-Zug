#ifndef CCONTEXT_H
#define CCONTEXT_H

#include <iostream>
#include <map>
#include <vector>
#include "func.hpp"
#include "Webcmd.hpp"

using std::string; using std::map;
using std::vector;
class CPlayer; 
class CGame;

class CContext
{
protected:

    typedef std::pair<string, string> event;
    typedef map<string, vector<void(CContext::*)(string&, CPlayer*)>>eventmanager; 

    eventmanager m_eventmanager;
    bool m_permeable;
    bool m_curPermeable;
    bool m_block;

    std::string m_sName;

    std::string m_sHelp;

public: 
    
    CContext() {}
    virtual ~CContext() {}

    // *** Getter *** //
    bool getPermeable();
    std::string getName() { return m_sName; }

    // *** Setter *** //
    virtual void setGame(CGame*) {}
    void setName(std::string sName) { m_sName = sName; }

    void add_listener(string sEventType, void(CContext::*)(string&, CPlayer*));
    void add_listener(string sEventType, void(CContext::*)(string&, CPlayer*), size_t pos);
    void delete_listener(string sEventType, int num);

    virtual bool throw_event(event newEvent, CPlayer* p);
    virtual void error(CPlayer* p) {}

    // *** EVENTHANDLERS *** // 
    void h_help(string&, CPlayer*);

    // *** STANDARD CONTEXT *** //
    virtual void h_find(string&, CPlayer*) {}
    virtual void h_show             (string&, CPlayer*) {} 
    virtual void h_look             (string&, CPlayer*) {}
    virtual void h_take             (string&, CPlayer*) {}
    virtual void h_consume          (string&, CPlayer*) {}
    virtual void h_equipe           (string&, CPlayer*) {}
    virtual void h_dequipe          (string&, CPlayer*) {}
    virtual void h_examine          (string&, CPlayer*) {}
    virtual void h_goTo             (string&, CPlayer*) {}
    virtual void h_startDialog      (string&, CPlayer*) {}
    virtual void h_changeMode       (string&, CPlayer*) {}

    //Rooms
    virtual void h_firstZombieAttack (string&, CPlayer*) {}
    virtual void h_moveToHospital    (string&, CPlayer*) {}
    virtual void h_exitTrainstation  (string&, CPlayer*) {}

    //Tuturial
    virtual void h_startTutorial     (string&, CPlayer*) {}

    // *** GAME CONTEXT *** //
    virtual void h_reloadGame(string&, CPlayer*) {}
    virtual void h_reloadPlayer(string&, CPlayer*) {}
    virtual void h_reloadWorlds(string&, CPlayer*) {}
    virtual void h_reloadWorld(string&, CPlayer*) {}
    virtual void h_updatePlayers(string&, CPlayer*) {}

    // *** WORLD CONTEXT *** //
    virtual void h_deleteCharacter(string&, CPlayer*) {}
    virtual void h_addItem(string&, CPlayer*) {}
    virtual void h_recieveMoney(string&, CPlayer*) {}
    virtual void h_newFight(string&, CPlayer*) {}
    virtual void h_endFight(string&, CPlayer*) {}
    virtual void h_endDialog(string&, CPlayer*) {}
    virtual void h_gameover(string&, CPlayer*) {}

    // *** FIGHT CONTEXT *** //
    virtual void h_fight(string&, CPlayer*) {}

    // *** DIALOG CONTEXT *** //
    virtual void h_call(string&, CPlayer*) {}

    // *** TRADECONTEXT *** //
    virtual void h_chooseSell(string&, CPlayer*) {}
    virtual void h_chooseBuy(string&, CPlayer*) {}
    virtual void h_decideSell(string&, CPlayer*) {}
    virtual void h_decideBuy(string&, CPlayer*) {}
    virtual void h_sell(string&, CPlayer*) {}
    virtual void h_buy(string&, CPlayer*) {}
    virtual void h_switchLeft(string&, CPlayer*) {}
    virtual void h_switchRight(string&, CPlayer*) {}
    virtual void h_switchInventory(string&, CPlayer*) {}
    virtual void h_exit(string&, CPlayer*) {}

    // *** CHATCONTEXT *** //
    virtual void h_send(string&, CPlayer*) {}
    virtual void h_end(string&, CPlayer*) {}

    // *** CHOICE CONTEXT *** //
    virtual void h_select(string&, CPlayer*) {}
    virtual void h_choose_equipe(string&, CPlayer*) {}
    virtual void h_updateStats(string&, CPlayer*) {}

    // *** QUESTS *** //
    virtual void h_ticketverkaeufer(string&, CPlayer*) {}
    virtual void h_ticketverkauf(string&, CPlayer*) {}
    virtual void h_zum_gleis(string&, CPlayer*) {}
    virtual void h_reden(string&, CPlayer*) {}
    virtual void h_besiege_besoffene_frau(string&, CPlayer*) {}
    virtual void h_geldauftreiben(string&, CPlayer*) {}

    // *** PROGRAMMER *** //
    virtual void h_try(string&, CPlayer*) {}

};



#endif

