#ifndef CDIALOGCONTEXT_H
#define CDIALOGCONTEXT_H

#include "CContext.hpp"

class CDialogContext : public CContext
{
private: 
    string m_curState;
    std::string m_dialogPartner;
public:
    CDialogContext(CPlayer* p, std::string partner);

    //Addhandlers
    void setCurState(std::string, CPlayer*);;

    //Handlers
    void h_call(string&, CPlayer*);

    void error(CPlayer*);
};

#endif
