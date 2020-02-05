#ifndef CCHOICECONTEXT_H
#define CCHOICECONTEXT_H

#include "CContext.hpp"

class CChoiceContext : public CContext
{
private:
    string m_sObject;
public:
    CChoiceContext(string obj);

    void h_choose_equipe(string&, CPlayer*);
};

#endif
