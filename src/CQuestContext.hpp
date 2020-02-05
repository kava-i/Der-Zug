#ifndef CQUESTCONTEXT_H
#define CQUESTCONTEXT_H

#include "CContext.hpp"
#include "CQuest.hpp"
#include "fuzzy.hpp"

class CQuestContext : public CContext
{
private:
    CQuest* m_quest;

    static std::map<string, void(CContext::*)(std::string&,CPlayer*)> m_functions;
    
public:
    CQuestContext(CQuest* quest);

    static void initializeFunctions();

    //Handler
    void h_ticketverkaeufer(std::string&, CPlayer*);
    void h_ticketverkauf(std::string&, CPlayer*);
    void h_zum_gleis(std::string&, CPlayer*);

}; 
     
#endif
