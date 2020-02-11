#ifndef CCHOICECONTEXT_H
#define CCHOICECONTEXT_H

#include "CContext.hpp"

class CChoiceContext : public CContext
{
private:
    string m_sObject;
    string m_sError;
public:
    CChoiceContext(string obj, string sError);

    void h_choose_equipe(string&, CPlayer*);
    void h_updateMind(string&, CPlayer*);
    void error(CPlayer*); 
};

#endif
