#ifndef CCHATCONTEXT_H
#define CCHATCONTEXT_H

#include "CContext.hpp"

class CChatContext : public CContext
{
private:
    CPlayer* m_chatPartner;
     
public:
    CChatContext(CPlayer* chatPartner)
    {
        //Set permeability
        m_permeable = false;
        
        //Set dialog partner
        m_chatPartner = chatPartner;

        //Add listeners
        add_listener("send", &CContext::h_send);
        add_listener("help", &CContext::h_help);
        add_listener("endChat", &CContext::h_end);
    }

    //Parser
    vector<event> parser(string, CPlayer*);

    //Handler
    void h_send(string&, CPlayer*);
    void h_end(string&, CPlayer*);
};

#endif
