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

// *** ITEM AND INVENTORY *** //
std::string CPerson::printInventory(std::string color, int highlight)
{
    std::string sOutput = color + m_sName + "'s Inventar: \n";

    if(m_inventory.size() == 0)
        return sOutput + "Inventar leer.\n";

    auto lamda1 = [](std::string x, std::vector<CItem*> vec) { 
                    return std::to_string(vec.size()) + "x " + x; };
    auto lamda2 = [](std::string x, std::vector<CItem*> vec) { 
                    return std::to_string(vec[0]->getValue()); };

    sOutput += table(m_inventory, lamda1, lamda2, "width:auto;border:1px solid black", highlight); 
    return sOutput;
}

void CPerson::addItem(CItem* item) {
    std::string sType = item->getAttribute<string>("type");
    sType.erase(sType.find("_"));
    m_inventory[sType][item->getName()].push_back(item); 
}

void CPerson::removeItem(string sItemName)
{
    for(auto it : m_inventory) {
        if(it.second.count(sItemName) > 0) {
            it.second[sItemName].pop_back();
            if(it.second[sItemName].size() == 0)
                m_inventory.erase(sItemName);
        }
    }
}

CItem* CPerson::getItem(string sName)
{
    for(auto it : m_inventory) {
        for(auto jt : it.second) {
            if(fuzzy::fuzzy_cmp(jt.second[0]->getName(), sName) <= 0.2) 
                return jt.second[0];
            }
    }
    return NULL;
}

CItem* CPerson::getItem_byID(string sID)
{
    for(auto it : m_inventory) {
        for(auto jt : it.second) {
            if(jt.second[0]->getID() == sID)
                return jt.second[0];
        }
    }
    return NULL;
}

// *** ATTACKS *** //
string CPerson::printAttacks()
{
    string sOutput = "Attacks: \n";
    size_t counter = 1;
    for(auto attack : m_attacks) {
        sOutput += std::to_string(counter) + ". \"" + attack.second->getName() + "\"\n"
                    + "--- Strength: " + std::to_string(attack.second->getPower()) + "\n"
                    + "--- " + attack.second->getDescription() + "\n";
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
    
