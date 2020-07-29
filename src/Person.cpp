#include "CPerson.hpp"
#include "CPlayer.hpp"


/**
* Constructor for Person, i.e. "character". Calls constructor from base class CObject.
* param[in] jAtts json with attributes
* param[in] dialogue dialogue of this character.
* param[in] items list of items 
* param[in] attacks list of attacks
*/
CPerson::CPerson(nlohmann::json jAttributes, CDialog* dialogue, attacks newAttacks, CText* text, CPlayer* p, std::map<std::string, CDialog*> dialogs, map_type items) : CObject(jAttributes, p)
{
    //Set stats.
    m_stats["highness"]	= 0;
    m_stats["hp"]	    = jAttributes.value("hp", 40);
    m_stats["max_hp"]   = m_stats["hp"];
    m_stats["gold"]	    = jAttributes.value("gold", 5);
    m_stats["strength"] = jAttributes.value("strength", 8);
    m_stats["skill"]    = jAttributes.value("skill", 8);
    m_stats["ep"]	    = jAttributes.value("ep", 0);
    
    
    m_roomDescription = new CText(jAttributes.value("roomDescription", nlohmann::json::parse("{}")), p);

    if(text!=nullptr)
        m_text = text;

    //Set dialogue and attacks
    m_attacks = newAttacks;
    m_dialog = dialogue;
    m_dialogs = dialogs;

    //Add items to inventory one by one
    for(auto &item : items)
        m_inventory.addItem(item.second);
}

// *** GETTER *** //

///return map of stats
std::map<std::string, int>& CPerson::getStats() {
    return m_stats;
}

/**
* Return person's stats, like strength, virtue, live-points, by passing name of stat.
* @param[in] id specify which stat shall be returned.
* @return return given stat.
*/
int CPerson::getStat(std::string id) { 
    if(m_stats.count(id) > 0)
        return m_stats[id]; 
    std::cout << "Attribute accessed which does not exits.\n";
    return 999;
}

///Return person's dialogue.
CDialog* CPerson::getDialog() { 
    return m_dialog; 
}

CDialog* CPerson::getDialog(std::string dialog) {
    return m_dialogs[dialog];
}

///Return person's attacks.
CPerson::attacks& CPerson::getAttacks() { 
    return m_attacks; 
}

///Return attacks as a map_type (string)
std::map<std::string, std::string> CPerson::getAttacks2() 
{
    auto lambda = [](CAttack* attack) { return attack->getName(); };
    return func::convertToObjectmap(m_attacks, lambda);
}

///Return person's inventory.
CInventory& CPerson::getInventory()  {
    return m_inventory; 
}

///Description where the player is located in the room
std::string CPerson::getRoomDescription() {
    return m_roomDescription->reducedPrint();
}

///Brief description of character
std::string CPerson::getReducedDescription() {
    return m_text->reducedPrint(false);
}


// *** SETTER *** //

///Set a new stat of this person
void CPerson::setStat(std::string id, int stat) {
    m_stats[id] = stat;
}

///Set person's dialogue.
void CPerson::setDialog(CDialog* newDialog) { 
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

string CPerson::printAttacksFight()
{
    std::string sOutput;
    //Iterate over attacks and add to output.
    for(auto[i, it] = std::tuple{1, m_attacks.begin()};it!=m_attacks.end();i++, it++) 
    {
        sOutput += "<span style=\"color: #c39bd3\">"+ std::to_string(i)+". "+it->second->getName() + " </span>"
		    + "[<span style=\"color: #f7dc6f;\">Power: " + std::to_string(it->second->getPower()) + "</span>]\n" + it->second->getDescription() + "\n\n";
    }

    //Return output.
    return sOutput;
}

string CPerson::getAttack(string sPlayerChoice)
{
    auto lambda = [](CAttack* attack) { return attack->getName(); };
    std::string sAttack = func::getObjectId(m_attacks, sPlayerChoice, lambda);

    if(m_attacks.count(sAttack) > 0)
        return sAttack;

    std::cout << "Error, attack accessed, that does not exist: " << sPlayerChoice << std::endl;
    return "";
}
    

// *** Various functions *** //

/**
* Check if a given attribute exists.
* @param[in] name of attribute.
* @return whether attribute exists or not.
*/
bool CPerson::attributeExists(std::string sAttribute) {
    return m_stats.count(func::returnToLower(sAttribute)) > 0;
}

std::string CPerson::getAllInformation()
{
    std::string sOutput = "";
    sOutput += "id: " + m_sID + ", name: " + m_sName + ", ";
    for(auto it : m_stats)
        sOutput += it.first + ": " + std::to_string(it.second) + ", ";
    sOutput + ", ";
    return sOutput;
}
