#ifndef CCONTEXT_H
#define CCONTEXT_H

#include <iostream>
#include <map>
#include <vector>
#include "func.hpp"

using std::string;
using std::map;
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

public: 
    
    CContext() {}
    virtual ~CContext() {}

    // *** Setter *** //
    bool getPermeable();

    // *** Setter *** //
    virtual void setGame(CGame*);

    void add_listener(string sEventType, void(CContext::*)(string&, CPlayer*));
    void add_listener(string sEventType, void(CContext::*)(string&, CPlayer*), size_t pos);
    void delete_listener(string sEventType, int num);

    void throw_event(string, CPlayer* p);

    // *** PARSER *** //
    virtual vector<event> parser(string, CPlayer*) { return {}; }


    // *** EVENTHANDLERS *** // 
    void h_help(string&, CPlayer*);

    // *** STANDARD CONTEXT *** //
    virtual void h_show             (string&, CPlayer*) {} 
    virtual void h_examine          (string&, CPlayer*) {} 
    virtual void h_lookIn           (string&, CPlayer*) {}
    virtual void h_take             (string&, CPlayer*) {}
    virtual void h_consume          (string&, CPlayer*) {}
    virtual void h_equipe           (string&, CPlayer*) {}
    virtual void h_dequipe          (string&, CPlayer*) {}
    virtual void h_goTo             (string&, CPlayer*) {}
    virtual void h_startDialog      (string&, CPlayer*) {}
    virtual void h_error            (string&, CPlayer*) {}

    //Rooms
    virtual void h_firstZombieAttack (string&, CPlayer*) {}
    virtual void h_moveToHospital    (string&, CPlayer*) {}
    virtual void h_endTutorial       (string&, CPlayer*) {}

    //Tuturial
    virtual void h_startTutorial     (string&, CPlayer*) {}

    // *** GAME CONTEXT *** //
    virtual void h_reloadGame(string&, CPlayer*);
    virtual void h_reloadPlayer(string&, CPlayer*);
    virtual void h_reloadWorld(string&, CPlayer*);
    virtual void h_updatePlayers(string&, CPlayer*);

    // *** WORLD CONTEXT *** //
    virtual void h_deleteCharacter(string&, CPlayer*) {}
    virtual void h_addItem(string&, CPlayer*) {}
    virtual void h_newFight(string&, CPlayer*) {}
    virtual void h_endFight(string&, CPlayer*) {}
    virtual void h_endDialog(string&, CPlayer*) {}

    // *** FIGHT CONTEXT *** //
    virtual void h_choose(string&, CPlayer*) {}

    // *** DIALOG CONTEXT *** //
    virtual void h_call(string&, CPlayer*) {}

    // *** CHATCONTEXT *** //
    virtual void h_send(string&, CPlayer*) {}
    virtual void h_end(string&, CPlayer*) {}

    // *** CHOICE CONTEXT *** //
    virtual void h_choose_equipe(string&, CPlayer*) {}

    // *** PROGRAMMER *** //
    virtual void h_try(string&, CPlayer*) {}

};



#endif


