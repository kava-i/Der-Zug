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
    if(std::isdigit(sPlayerChoice[0]) == false)
        return "";

    int counter=1;
    for(auto it : m_attacks) {
        if(counter == stoi(sPlayerChoice))
            return it.first;
         counter++;
    }

    return "";
}
    
