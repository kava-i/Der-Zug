#include "CItem.hpp"
#include "CPlayer.hpp"

CItem::CItem(nlohmann::json jBasic)
{
    m_jAtts = jBasic;
}

CItem::CItem(nlohmann::json jBasic, nlohmann::json jItem)
{
    m_jAtts = jBasic;

    for(auto it=jItem.begin(); it!=jItem.end(); ++it) {
        if(it.key() == "from")
            continue;
        m_jAtts[it.key()] = it.value();
    }
}

// *** GETTTER *** //
string CItem::getID()      { return getAttribute<string>("id"); }
string CItem::getName()    { return getAttribute<string>("name"); }
string CItem::getFunction(){ return m_jAtts.value("function", m_jAtts["type"]); } 
size_t CItem::getEffekt()  { return getAttribute<size_t>("effekt"); }


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
    p->setHighness(p->getHighness() + getEffekt());
    if(!p->checkEventExists("highness"))
        p->addTimeEvent("highness", 2, &CPlayer::t_highness);
    p->removeItem(m_jAtts["name"]);
    p->appendPrint("You consume drug: " + getName() + ". Highness inceased by " + std::to_string(getEffekt()) + ".\n");
}


// ***** EQUIPE-FUNCTIONS ***** //
void CItem::equipeWeapon(CPlayer* p)
{
    string sType = func::split(m_jAtts["type"], "_")[1];
    p->equipeItem(this, sType);
}


