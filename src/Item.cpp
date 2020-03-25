#include "CItem.hpp"
#include "CPlayer.hpp"

// *** GETTTER *** //
nlohmann::json CItem::getAttributes() { return m_jAtts; }
string CItem::getDescription() { return getAttribute<string>("description"); }
string CItem::getFunction(){ return m_jAtts.value("function", m_jAtts["type"]); } 
size_t CItem::getEffekt()  { return getAttribute<size_t>("effekt"); }
int CItem::getValue()      { return getAttribute<int>("value"); }


// Initialize Functions 
std::map<string, void (CItem::*)(CPlayer* p)> CItem::m_functions= {};
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
    string sType = func::split(m_jAtts["type"], "_")[1];
    p->equipeItem(this, sType);
}


