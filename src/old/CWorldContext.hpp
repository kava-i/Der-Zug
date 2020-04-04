#ifndef CWORLDCONTEXT_H
#define CWORLDCONTEXT_H

#include "CContext.hpp"

class CWorldContext : public CContext
{
public:
    CWorldContext();

    //Parser
    vector<event> parser(string, CPlayer*);

    //Handler
    void h_deleteCharacter(string&, CPlayer*);
    void h_addItem(string&, CPlayer*);
    void h_recieveMoney(string&, CPlayer*);
    void h_newFight(string&, CPlayer*);
    void h_endFight(string&, CPlayer*);
    void h_endDialog(string&, CPlayer*);
    void h_gameover(string&, CPlayer*);
};

#endif
