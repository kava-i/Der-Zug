#ifndef CFIGHTCONTEXT_H
#define CFIGHTCONTEXT_H

#include "CContext.hpp"

class CAttack;

class CFightContext : public CContext
{
public: 
    CFightContext(std::map<std::string, CAttack*> attacks)
    {
        //Set permeability
        m_permeable = false;

        //Add listeners
        add_listener("show", &CContext::h_show);
        add_listener("help", &CContext::h_help);
        
        addHandlers(attacks);
    }

    //Addhandlers
    void addHandlers(std::map<string, CAttack*> attacks);

    //Handlers
    void h_fight(string&, CPlayer*);
    void h_show(string&, CPlayer*);

    void error(CPlayer* p);
};

#endif
