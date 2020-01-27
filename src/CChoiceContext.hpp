#ifndef CCHOICECONTEXT_H
#define CCHOICECONTEXT_H

#include "CContext.hpp"

class CChoiceContext : public CContext
{
private:
    string m_sObject;
public:
    CChoiceContext(string obj)
    {
        //Set permeability
        m_permeable = false;

        //Set object
        m_sObject=obj;
    }

    //Parser
    vector<event> parser(string, CPlayer*);

    void h_choose_equipe(string&, CPlayer*);
};

#endif
