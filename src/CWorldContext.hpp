#ifndef CWORLDCONTEXT_H
#define CWORLDCONTEXT_H

#include "CContext.hpp"

class CWorldContext : public CContext
{
public:
    CWorldContext() 
    {
        //Set permeability
        m_permeable = true;

        //Set jandlers
        add_listener("deleteCharacter", &CContext::h_deleteCharacter);
        add_listener("addItem", &CContext::h_addItem);
        add_listener("newFight", &CContext::h_newFight);
        add_listener("endFight", &CContext::h_endFight);
        add_listener("endDialog", &CContext::h_endDialog);
    }

    //Parser
    vector<event> parser(string, CPlayer*);

    //Handler
    void h_deleteCharacter(string&, CPlayer*);
    void h_addItem(string&, CPlayer*);
    void h_newFight(string&, CPlayer*);
    void h_endFight(string&, CPlayer*);
    void h_endDialog(string&, CPlayer*);
};

#endif
