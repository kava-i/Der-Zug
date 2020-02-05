#ifndef CFIGHTCONTEXT_H
#define CFIGHTCONTEXT_H

#include "CContext.hpp"

class CAttack;

class CFightContext : public CContext
{
public: 
    CFightContext();

    //Addhandlers
    void addHandlers(std::map<string, CAttack*> attacks);

    //Handlers
    void h_fight(string&, CPlayer*);
    void h_show(string&, CPlayer*);

    void error(CPlayer* p);
};

#endif
