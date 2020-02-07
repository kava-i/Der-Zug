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

    // *** zug nach Moskau *** //
    void h_ticketverkaeufer(std::string&, CPlayer*);
    void h_ticketverkauf(std::string&, CPlayer*);
    void h_zum_gleis(std::string&, CPlayer*);

    // *** besoffene frau *** //
    void h_besiege_besoffene_frau(std::string&, CPlayer*);
    
    // *** Geld auftreiben *** //
    void h_geldauftreiben(string&, CPlayer*);
}; 
     
#endif
