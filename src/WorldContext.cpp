#include "CWorldContext.hpp"
#include "CPlayer.hpp"

CWorldContext::CWorldContext()
{
    //Set permeability
    m_permeable = true;
    m_curPermeable = m_permeable;

    //Set jandlers
    add_listener("deleteCharacter", &CContext::h_deleteCharacter);
    add_listener("addItem", &CContext::h_addItem);
    add_listener("recieveMoney", &CContext::h_recieveMoney);
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

    m_curPermeable=false;
}

void CWorldContext::h_addItem(string& sIdentifier, CPlayer* p) {
    p->addItem(p->getWorld()->getItem(sIdentifier));
    m_curPermeable=false;
}

void CWorldContext::h_recieveMoney(string& sIdentifier, CPlayer* p) {
    p->setStat("gold", p->getStat("gold") + stoi(sIdentifier));
    p->appendPrint(Webcmd::set_color(Webcmd::color::GREEN) + "+" + sIdentifier + " Schiling" + Webcmd::set_color(Webcmd::color::WHITE) + "\n");

   m_curPermeable = false; 
}

void CWorldContext::h_endFight(string& sIdentifier, CPlayer* p) {
    std::cout << "h_endFight " << std::endl;
    p->endFight();
    m_curPermeable=false;
}

void CWorldContext::h_endDialog(string& sIdentifier, CPlayer* p) {
    p->getContexts().erase("dialog");
    m_curPermeable=false;
}


void CWorldContext::h_newFight(string& sIdentifier, CPlayer* p) {
    p->setFight(new CFight(p, p->getWorld()->getCharacters()[sIdentifier]));
}

void CWorldContext::h_gameover(string& sIdentifier, CPlayer* p) {
    p->setStat("hp", 0);
    p->appendPrint("\nDu bist gestorben... \n\n Press Enter to continue....\n");
}
