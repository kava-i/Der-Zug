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
    for(auto attack : m_attacks)
        sOutput += "-> \"" + attack.second->getName() + "\": " + attack.second->getDescription() + "\n";

    return sOutput;
}

string CPerson::getAttack(string sPlayerChoice)
{
    if(std::isdigit(sPlayerChoice[0]) == true)
    {
        int counter=1;
        for(auto it : m_attacks) {
            if(counter == stoi(sPlayerChoice))
                return it.first;
             counter++;
        }
    }

    else if(sPlayerChoice.size() > 2)
    {
        for(auto it : m_attacks) {
            if(fuzzy::fuzzy_cmp(it.second->getName(), sPlayerChoice) <= 0.2) 
                return it.first;
         }
    }

    return "";
}
    
