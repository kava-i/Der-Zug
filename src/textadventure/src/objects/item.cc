#include "item.h"
#include "objects/player.h"

/**
* Constructor for non-derived items simply placing the passed json as items attributes.
* @param[in] basic attributes from base item.
*/
CItem::CItem(nlohmann::json basic_json, CPlayer* p) : CObject(basic_json, p, "item") {
  m_jAttributes = basic_json;

  if (basic_json.count("useDescription") > 0)
    m_useDescription = new CText(basic_json["useDescription"], p);
  else
    m_useDescription = nullptr;

  m_sCategory = basic_json.value("category", "others");
  kind_ = basic_json.value("kind", "");
  type_ = basic_json.value("type", "");

  m_sAttack = basic_json.value("attack", "");
  m_sFunction = basic_json.value("function", m_sCategory);
  stats_change_ = basic_json.value("stats_effect", "");
  m_effekt = basic_json.value("effekt", 0);
  m_value = basic_json.value("value", 1);
  m_hidden = basic_json.value("hidden", false);

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
std::string CItem::kind() {
  return kind_;
}
std::string CItem::type() {
  return type_;
}
std::string CItem::getAttack() {
  return m_sAttack;
}
std::string CItem::getFunction() { 
  return m_sFunction;
} 
std::string CItem::stats_change() {
  return stats_change_;
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
void CItem::setKind(std::string kind) {
  kind_ = kind;
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
void CItem::initializeFunctions() {
  //Consume-functions
  m_functions["consume"] = &CItem::consume;
  //Read-functions
  m_functions["read"] = &CItem::read;
  //Equipe-functions
  m_functions["equipe"] = &CItem::equipe;
}

// ***** VARIOUS FUNCTIONS ***** //
std::string CItem::getAllInfos() {
  return "id: " + id_ + ", name: " + name_ + ", catagory: " + m_sCategory + ", kind: " + kind_ 
    + ", type: " + type_ + ", function: " + m_sFunction + ", attack: " + m_sAttack 
    + ", effekt: " + std::to_string(m_effekt) + ", value: " + std::to_string(m_value) 
    + ", hidden: " + std::to_string(m_hidden) + "\n"; 
}

// ***** FUNCTION-CALLER ***** // 

//Consume function
bool CItem::callFunction(CPlayer* p) {
  if (m_functions.count(getFunction()) == 0) 
    return false;
  else
    (this->*m_functions[getFunction()])(p);
  return true;
}

// ***** CONSUME FUNCTIONS ***** //
void CItem::consume(CPlayer* p) {
  if (kind_ == "drug") {
    p->setStat("highness", p->getStat("highness") + getEffekt());
    if (!p->getContext("standard")->timeevent_exists("t_highness"))
        p->getContext("standard")->add_timeEvent("t_highness", "standard", "", 2.0);
    if (m_useDescription != nullptr)
        p->appendPrint(m_useDescription->print());
    else
        p->appendDescPrint("Du consumierst eine Droge: deine Highness erhöht sich um " 
            + std::to_string(getEffekt()));
    p->getInventory().removeItemByID(id());
  }
}


// ***** EQUIPE-FUNCTIONS ***** //
void CItem::equipe(CPlayer* p) {
  p->equipeItem(this, kind_);
}

// ***** READ-FUNCTIONS ***** //
void CItem::read(CPlayer* p) {
  p->appendDescPrint("Du schlägst das Buch auf der ersten Seite auf.");
  p->appendPrint(m_pages->pagePrint(m_mark) + "\n");
  p->addReadContext(id_);
}
