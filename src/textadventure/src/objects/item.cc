#include "item.h"
#include "objects/player.h"

/**
* Constructor for non-derived items simply placing the passed json as items attributes.
* @param[in] basic attributes from base item.
*/
CItem::CItem(nlohmann::json jBasic, CPlayer* p) : CObject{jBasic, p}
{
    m_jAttributes = jBasic;

    if(jBasic.count("useDescription") > 0)
        m_useDescription = new CText(jBasic["useDescription"], p);
    else
        m_useDescription = nullptr;

    std::string sType = jBasic.value("type", "");
    m_sCategory = jBasic.value("category", "others");
    m_sType = jBasic.value("type", "");

    m_sAttack = jBasic.value("attack", "");
    m_sFunction = jBasic.value("function", m_sCategory);
    m_effekt = jBasic.value("effekt", 0);
    m_value = jBasic.value("value", 1);
    m_hidden = jBasic.value("hidden", false);

    //Set extra variables for books
    m_mark = 0;
    m_pages = new CText(m_jAttributes.value("pages", nlohmann::json()), p);
}

// *** GETTTER *** //
nlohmann::json CItem::getAttributes() {
    return m_jAttributes;
}
std::string CItem::getCategory() {
    return m_sCategory;
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
size_t CItem::getMark() {
    return m_mark;
}
CText* CItem::getPages() {
    return m_pages;
}

// *** SETTER *** //
void CItem::setCategory(std::string sCategory) {
    m_sCategory = sCategory;
}
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
void CItem::setMark(size_t mark) {
    m_mark = mark;
}


// Initialize Functions 
std::map<std::string, void (CItem::*)(CPlayer* p)> CItem::m_functions= {};
void CItem::initializeFunctions()
{
    //Consume-functions
    m_functions["consume"] = &CItem::consume;

    //Read-functions
    m_functions["read"] = &CItem::read;

    //Equipe-functions
    m_functions["equipe"] = &CItem::equipe;
}

// ***** VARIOUS FUNCTIONS ***** //
std::string CItem::getAllInfos()
{
    return "id: " + m_sID + ", name: " + m_sName + ", catagory: " + m_sCategory + ", type: " + m_sType + ", function: " + m_sFunction + ", attack: " + m_sAttack + ", effekt: " + std::to_string(m_effekt) + ", value: " + std::to_string(m_value) + ", hidden: " + std::to_string(m_hidden) + "\n"; 
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
void CItem::consume(CPlayer* p)
{
    if(m_sType == "drug")
    {
        p->setStat("highness", p->getStat("highness") + getEffekt());
        if(!p->getContext("standard")->timeevent_exists("t_highness"))
            p->getContext("standard")->add_timeEvent("t_highness", "standard", "", 0.2);
        if(m_useDescription != nullptr)
            p->appendPrint(m_useDescription->print());
        else
            p->appendDescPrint("Du consumierst eine Droge: deine Highness erhöht sich um " + std::to_string(getEffekt()));
        p->getInventory().removeItemByID(getID());
    }
}


// ***** EQUIPE-FUNCTIONS ***** //
void CItem::equipe(CPlayer* p)
{
    p->equipeItem(this, m_sType);
}

// ***** READ-FUNCTIONS ***** //
void CItem::read(CPlayer* p)
{
    p->appendDescPrint("Du schlägst das Buch auf der ersten Seite auf.");
    p->appendPrint(m_pages->pagePrint(m_mark) + "\n");
    p->addReadContext(m_sID);
}
