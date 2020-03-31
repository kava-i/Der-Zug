#include "CPerson.hpp"


/**
* Constructor for Person, i.e. "character". Calls constructor from base class CObject.
* param[in] jAtts json with attributes
* param[in] dialogue dialogue of this character.
* param[in] items list of items 
* param[in] attacks list of attacks
*/
CPerson::CPerson(nlohmann::json jAttributes, SDialog* dialogue, attacks newAttacks, CPlayer* p, map_type items) : CObject(jAttributes, p)
{
    //Set stats.
    m_stats["highness"] = 0;
    m_stats["hp"]       = jAttributes.value("hp", 40);
    m_stats["gold"]     = jAttributes.value("gold", 5);
    m_stats["strength"] = jAttributes.value("strength", 8);
    m_stats["skill"]    = jAttributes.value("skill", 8);
    

    //Set dialogue and attacks
    m_attacks = newAttacks;
    m_dialog = dialogue;

    //Add items to inventory one by one
    for(auto &item : items)
        m_inventory.addItem(item.second);
}

// *** GETTER *** //

/**
* Return person's stats, like strength, virtue, live-points, by passing name of stat.
* @param[in] id specify which stat shall be returned.
* @return return given stat.
*/
int CPerson::getStat(std::string id) { 
    return m_stats[id]; 
}

///Return person's dialogue.
SDialog* CPerson::getDialog() { 
    return m_dialog; 
}

///Return person's attacks.
CPerson::attacks& CPerson::getAttacks() { 
    return m_attacks; 
}

///Return person's inventory.
CInventory& CPerson::getInventory()  {
    return m_inventory; 
}


// *** SETTER *** //

///Set a new stat of this person
void CPerson::setStat(std::string id, int stat) {
    m_stats[id] = stat;
}

///Set person's dialogue.
void CPerson::setDialog(SDialog* newDialog) { 
    m_dialog = newDialog; 
}


// *** ATTACKS *** //

/**
* Print all attacks. Attacks are printed in the form: Name \n Strengt\n Description.
*/
string CPerson::printAttacks()
{
    string sOutput = "Attacks: \n";

    //Iterate over attacks and add to output.
    for(auto[i, it] = std::tuple{1, m_attacks.begin()};it!=m_attacks.end();i++, it++) 
    {
        sOutput += std::to_string(i) + ". \"" + it->second->getName() + "\"\n"
                    + "--- Strength: " + std::to_string(it->second->getPower()) + "\n"
                    + "--- " + it->second->getDescription() + "\n";
    }

    //Return output.
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
    
