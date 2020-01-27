#ifndef CDIALOGCONTEXT_H
#define CDIALOGCONTEXT_H

#include "CContext.hpp"

class CDialogContext : public CContext
{
private: 
    string m_curState;
public:
    CDialogContext()
    {
        //Set permeability
        m_permeable = false;

        //Set first state
        m_curState="START";

        //Add listeners
        add_listener("call", &CContext::h_call); 
        add_listener("error", &CContext::h_error);
        add_listener("choose", &CContext::h_call);
        add_listener("help", &CContext::h_help);
    }

    //Parser 
    vector<event> parser(string, CPlayer*);

    //Handlers
    void h_call(string&, CPlayer*);
    void h_error(string&, CPlayer*);
};

#endif
