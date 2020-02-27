#include "CCharacter.hpp"

CCharacter::CCharacter(nlohmann::json jAtts, SDialog* newDialog, std::map<std::string, CItem*> mapItems, attacks newAttacks, CPlayer* p) {

    m_sName = jAtts["name"];
    m_sID = jAtts["id"];
    m_text = new CText(jAtts.value("description", nlohmann::json::array()), p);
    
    //Stats
    m_stats["hp"] = jAtts.value("hp", 30);
    m_stats["strength"] = jAtts.value("strength",7);

    m_dialog = newDialog;
    m_attacks = newAttacks;

    for(auto &item : mapItems)
        addItem(item.second);
}

// *** GETTER *** //
string CCharacter::getDescription() {return m_text->print(); }


