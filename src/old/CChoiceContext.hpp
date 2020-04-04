#ifndef CCHOICECONTEXT_H
#define CCHOICECONTEXT_H

#include "CContext.hpp"
#include "json.hpp"

class CChoiceContext : public CContext
{
private:

    typedef std::map<std::string, std::string> objectmap;
    objectmap m_objectMap;
    string m_sObject;
    string m_sError;
public:
    CChoiceContext(string obj, string sError="Wrong Input!\n");
    CChoiceContext(std::string obj, objectmap& mapObjects);

    void h_select(string&, CPlayer*);
    void h_choose_equipe(string&, CPlayer*);
    void h_updateStats(string&, CPlayer*);
    void error(CPlayer*); 
};

#endif
