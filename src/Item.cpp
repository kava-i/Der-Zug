#include "CItem.hpp"
#include "CPlayer.hpp"

// *** GETTTER *** //
nlohmann::json CItem::getAttributes() {
    return m_jAttributes;
}
std::string CItem::getType() {
    return m_sType;
}
std::string CItem::getAttack() {
    return m_sAttack;
}
std::string CItem::getFunction() { 
    return m_sFunction;
} 
size_t CItem::getEffekt() { 
    return m_effekt;
}
int CItem::getValue() { 
    return m_value;
}
bool CItem::getHidden() {
    return m_hidden;
}

// *** SETTER *** //
void CItem::setType(std::string sType) {
    m_sType = sType;
}
void CItem::setFunction(std::string sFunction) {
    m_sFunction = sFunction;
}
void CItem::setAttack(std::string sAttack) {
    m_sAttack = sAttack;
}
void CItem::setEffekt(size_t effekt) {
    m_effekt = effekt;
}
void CItem::setValue(int value) {
    m_value = value;
}
void CItem::setHidden(bool hidden) {
    m_hidden = hidden;
}


// Initialize Functions 
std::map<std::string, void (CItem::*)(CPlayer* p)> CItem::m_functions= {};
void CItem::initializeFunctions()
{
    //Consume-functions
    m_functions["consume_drug"] = &CItem::consumeDrug;

    //Equipe-functions
    m_functions["equipe_weapon"] = &CItem::equipeWeapon;
}

// ***** FUNCTION-CALLER ***** // 

//Consume function
bool CItem::callFunction(CPlayer* p) {
    if(m_functions.count(getFunction()) == 0) 
        return false;
    else
        (this->*m_functions[getFunction()])(p);
    return true;
}


// ***** CONSUME FUNCTIONS ***** //
void CItem::consumeDrug(CPlayer* p)
{
    p->setStat("highness", p->getStat("highness") + getEffekt());
    if(!p->checkEventExists("highness"))
        p->addTimeEvent("highness", 2, &CPlayer::t_highness);
    p->appendPrint("You consume drug: " + getName() + ". Highness inceased by " + std::to_string(getEffekt()) + ".\n");
    p->getInventory().removeItemByID(getID());
}


// ***** EQUIPE-FUNCTIONS ***** //
void CItem::equipeWeapon(CPlayer* p)
{
    p->equipeItem(this, func::split(m_sType, "_")[1]);
}


