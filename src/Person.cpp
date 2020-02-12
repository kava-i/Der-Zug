#include "CPerson.hpp"

// *** GETTER *** //

string CPerson::getName()       { return m_sName; }
string CPerson::getID()         { return m_sID; }
int CPerson::getHp()            { return m_hp; }
size_t CPerson::getStrength()   { return m_strength; }
int CPerson::getGold()       { return m_gold; }
SDialog* CPerson::getDialog()   { return m_dialog; }
CPerson::attacks& CPerson::getAttacks() { return m_attacks; }

// *** SETTER *** //
void CPerson::setHp(int hp) { m_hp = hp; }
void CPerson::setDialog(SDialog* newDialog) { m_dialog = newDialog; }
void CPerson::setGold(int gold) { m_gold = gold; }

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
    
