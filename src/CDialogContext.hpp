#ifndef CDIALOGCONTEXT_H
#define CDIALOGCONTEXT_H

#include "CContext.hpp"

class CDialogContext : public CContext
{
private: 
    string m_curState;
public:
    CDialogContext();

    //Parser 
    vector<event> parser(string, CPlayer*);

    //Handlers
    void h_call(string&, CPlayer*);
    void h_error(string&, CPlayer*);
};

#endif
