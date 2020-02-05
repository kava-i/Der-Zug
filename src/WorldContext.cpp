#include "CWorldContext.hpp"
#include "CPlayer.hpp"

CWorldContext::CWorldContext()
{
    //Set permeability
    m_permeable = true;

    //Set jandlers
    add_listener("deleteCharacter", &CContext::h_deleteCharacter);
    add_listener("addItem", &CContext::h_addItem);
    add_listener("fight", &CContext::h_newFight);
    add_listener("endFight", &CContext::h_endFight);
    add_listener("endDialog", &CContext::h_endDialog);
    add_listener("gameover", &CContext::h_gameover);
}

// **** HANLDERS ***** //

void CWorldContext::h_deleteCharacter(string& sIdentifier, CPlayer* p) {
    std::cout << "h_deleteCharacter " << sIdentifier << std::endl;
    p->getRoom()->getCharacters().erase(sIdentifier);
    delete p->getWorld()->getCharacters()[sIdentifier];
    p->getWorld()->getCharacters().erase(sIdentifier); 

    m_permeable=false;
}

void CWorldContext::h_addItem(string& sIdentifier, CPlayer* p) {
    p->addItem(p->getWorld()->getItem(sIdentifier));
    m_permeable=false;
}

void CWorldContext::h_endFight(string& sIdentifier, CPlayer* p) {
    std::cout << "h_endFight " << std::endl;
    p->endFight();
    m_permeable=false;
}

void CWorldContext::h_endDialog(string& sIdentifier, CPlayer* p) {
    p->getContexts().erase("dialog");
    m_permeable=false;
}


void CWorldContext::h_newFight(string& sIdentifier, CPlayer* p) {
    p->setFight(new CFight(p, p->getWorld()->getCharacters()[sIdentifier]));
}

void CWorldContext::h_gameover(string& sIdentifier, CPlayer* p) {
    p->setHp(0);
    p->appendPrint("\nDu bist gestorben... \n\n Press Enter to continue....\n");
}

void CWorldContext::CWorldContext::error(CPlayer* ) {
    m_permeable = true;
}
