#include "CPerson.hpp"

// *** GETTER *** //

string CPerson::getName()       { return m_sName; }
string CPerson::getID()         { return m_sID; }
int CPerson::getStat(std::string id)     { return m_stats[id]; }
SDialog* CPerson::getDialog()   { return m_dialog; }
CPerson::attacks& CPerson::getAttacks() { return m_attacks; }

// *** SETTER *** //
void CPerson::setStat(std::string id, int stat) {
    m_stats[id] = stat;
}
void CPerson::setDialog(SDialog* newDialog) { 
    m_dialog = newDialog; 
}

string CPerson::printAttacks()
{
    string sOutput = "Attacks: \n";
    size_t counter = 1;
    for(auto attack : m_attacks) {
        sOutput += std::to_string(counter) + ". \"" + attack.second->getName() + "\": " + attack.second->getDescription() + "\n";
        counter++;
    }

    return sOutput;
}

string CPerson::getAttack(string sPlayerChoice)
{
    for(auto[i, it] = std::tuple{1, m_attacks.begin()};it!=m_attacks.end();i++, it++) {
        if(std::isdigit(sPlayerChoice[0]) && i==stoi(sPlayerChoice))
            return it->first;
    }
    return "";
}
    
