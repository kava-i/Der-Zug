#include "CItem.hpp"
#include "CPlayer.hpp"

/**
* Constructor for non-derived items simply placing the passed json as items attributes.
* @param[in] basic attributes from base item.
*/
CItem::CItem(nlohmann::json jBasic, CPlayer* p) : CObject{jBasic, p}
{
    std::cout << "Creating item... \n";
    m_jAttributes = jBasic;

    std::string sType = jBasic.value("type", "");
    std::cout << "Type: " << sType << std::endl;
    m_sCategory = func::split(sType, "_")[0];
    m_sType = func::split(sType, "_")[1];

    std::cout << "built item-category (" << m_sCategory << ") and type (" << m_sType << ")\n";

    m_sAttack = jBasic.value("attack", "");
    m_sFunction = jBasic.value("function", m_sCategory);
    m_effekt = jBasic.value("effekt", 0);
    m_value = jBasic.value("value", 1);
    m_hidden = jBasic.value("hidden", false);

    std::cout << "Finished creating item.\n";
}

/**
* Constructor for derived items. The basic json usually supplies basic attributes. Which 
* might get overridden, by specific attributes, which otherwise only supply extra item
* specific attributes.
* @param[in] jBasic basic attributes from base item.
* @param[in] jItem extra attributes from specific individual item.
*/
CItem::CItem(nlohmann::json jBasic, nlohmann::json jItem, CPlayer* p, std::string sID) : CObject {jBasic, p} 
{
    m_sID = sID;
    
    std::string sType = jItem.value("type", jBasic.value("type", ""));
    m_sCategory = func::split(sType, "_")[0];
    m_sType = func::split(sType, "_")[1];

    m_sAttack = jItem.value("attack", jBasic.value("attack", ""));
    m_sFunction = jItem.value("function", jBasic.value("function", m_sCategory));
    m_effekt = jItem.value("effekt", jBasic.value("effekt", 0));
    m_value = jItem.value("value", jBasic.value("value", 1));
    m_hidden = jItem.value("hidden", false);

    m_jAttributes = jBasic;
    jItem.erase("amount");
    for(auto it=jItem.begin(); it!=jItem.end(); ++it)
        m_jAttributes[it.key()] = it.value();
    m_jAttributes["id"] = m_sID;
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


// Initialize Functions 
std::map<std::string, void (CItem::*)(CPlayer* p)> CItem::m_functions= {};
void CItem::initializeFunctions()
{
    //Consume-functions
    m_functions["consume"] = &CItem::consume;

    //Equipe-functions
    m_functions["equipe"] = &CItem::equipe;
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
        if(!p->checkEventExists("highness"))
            p->addTimeEvent("highness", 2, &CPlayer::t_highness);
        p->appendPrint("You consume drug: " + getName() + ". Highness inceased by " + std::to_string(getEffekt()) + ".\n");
        p->getInventory().removeItemByID(getID());
    }
}


// ***** EQUIPE-FUNCTIONS ***** //
void CItem::equipe(CPlayer* p)
{
    p->equipeItem(this, m_sType);
}


