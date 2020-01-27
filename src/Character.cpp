#include "CCharacter.hpp"

CCharacter::CCharacter(string sName, string sID, string sDescription, int hp, size_t strength, SDialog* newDialog, attacks newAttacks) {

    m_sName = sName;
    m_sID = sID;
    m_sDescription = sDescription;
    m_hp = hp;
    m_strength = strength;
    m_dialog = newDialog;
    m_attacks = newAttacks;

}
// *** GETTER *** //
string CCharacter::getDescription() {return m_sDescription; }


