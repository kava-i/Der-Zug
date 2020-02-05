#include "CFightContext.hpp"
#include "CPlayer.hpp"

CFightContext::CFightContext(std::map<std::string, CAttack*> attacks)
{
    //Set permeability
    m_permeable = false;

    //Add listeners
    add_listener("show", &CContext::h_show);
    add_listener("help", &CContext::h_help);

    addHandlers(attacks);
}

void CFightContext::addHandlers(std::map<std::string, CAttack*> attacks)
{
    size_t counter=1;
    for(auto it : attacks) {
        add_listener(std::to_string(counter), &CContext::h_fight);
        counter++;
    }
}


// ***** HANDLERS ***** //
void CFightContext::h_fight(string& sIdentifier, CPlayer* p) {
    std::cout << "h_fight " << sIdentifier << std::endl;
    std::string newCommand = p->getFight()->fightRound((sIdentifier));
    if(newCommand != "")    
        p->throw_event(newCommand);
}

void CFightContext::h_show(string& sIdentifier, CPlayer* p) {
    if(sIdentifier == "stats")
        p->appendPrint(p->getFight()->printStats(p));
    else if(sIdentifier == "opponent stats")
        p->appendPrint(p->getFight()->printStats(p->getFight()->getOpponent()));
}

void CFightContext::error(CPlayer* p) {
    p->appendPrint("Fight: I can't do this. Please choose an attack, or use help.\n");
}
