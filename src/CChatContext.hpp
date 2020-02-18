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
        m_curPermeable = m_permeable;
        
        //Set dialog partner
        m_chatPartner = chatPartner;

        //Add listeners
        add_listener("send", &CContext::h_send);
        add_listener("help", &CContext::h_help);
        add_listener("endChat", &CContext::h_end);
    }

    //Alternate throw_event
    void throw_event(event newEvent, CPlayer* p);
     
    //Handler
    void h_send(string&, CPlayer*);
    void h_end(string&, CPlayer*);
};

#endif
