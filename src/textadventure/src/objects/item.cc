#include "item.h"
#include "objects/helper/update.h"
#include "objects/player.h"
#include "tools/func.h"
#include <math.h>
#include <string>


/**
* Constructor for non-derived items simply placing the passed json as items attributes.
* @param[in] basic attributes from base item.
*/
CItem::CItem(nlohmann::json basic_json, CPlayer* p) 
  : CObject(basic_json, p, "item"),
  updates_(basic_json.value("updates", nlohmann::json::array())),
  costs_(Updates(basic_json.value("costs", std::map<std::string, int>()), calc::ModType::ADD))
{
  json_ = basic_json;

  if (basic_json.count("useDescription") > 0)
    use_description_ = new CText(basic_json["useDescription"], p);
  else
    use_description_ = nullptr;

  category_ = basic_json.value("category", "others");
  kind_ = basic_json.value("kind", "");
  type_ = basic_json.value("type", "");

  attack_ = basic_json.value("attack", "");
  hidden_ = basic_json.value("hidden", false);

  //Set extra variables for books
  m_mark = 0;
  m_pages = new CText(json_.value("pages", nlohmann::json()), p);
}

// *** GETTTER *** //
nlohmann::json CItem::getAttributes() {
  return json_;
}
std::string CItem::getCategory() {
  return category_;
}
std::string CItem::kind() {
  return kind_;
}
std::string CItem::type() {
  return type_;
}
std::string CItem::getAttack() {
  return attack_;
}

bool CItem::getHidden() {
  return hidden_;
}

size_t CItem::getMark() {
  return m_mark;
}

CText* CItem::getPages() {
  return m_pages;
}

const Updates& CItem::update() {
  return updates_;
}

const Updates& CItem::costs() {
  return costs_;
}

CText* CItem::use_description() {
  return use_description_;
}

// *** SETTER *** //
void CItem::setCategory(std::string sCategory) {
  category_ = sCategory;
}
void CItem::setKind(std::string kind) {
  kind_ = kind;
}

void CItem::setAttack(std::string sAttack) {
  attack_ = sAttack;
}

void CItem::setHidden(bool hidden) {
  hidden_ = hidden;
}
void CItem::setMark(size_t mark) {
  m_mark = mark;
}


// Initialize Functions 
std::map<std::string, void (CItem::*)(CPlayer* p)> CItem::m_functions= {};
void CItem::initializeFunctions() {
  // Consume-functions
  m_functions["consume"] = &CItem::consume;
  // Read-functions
  m_functions["read"] = &CItem::read;
  // Equipe-functions
  m_functions["equipe"] = &CItem::equipe;
  m_functions["weapon"] = &CItem::equipe;
}

// ***** VARIOUS FUNCTIONS ***** //
std::string CItem::getAllInfos() {
  return "id: " + id_ + ", name: " + name_ + ", catagory: " + category_ + ", kind: " + kind_ 
    + ", type: " + type_ + ", attack: " + attack_ + ", costs: " + Costs() + "\n"; 
}

std::string CItem::Costs() {
  auto updates = costs_.updates();
  if (updates.size() == 0) 
	  return "Costs: ---";
  auto lambda = [](const Update& c) -> std::string { 
    return c.id_ + ": " + func::dtos(c.value_); };
	return "Costs: " + std::accumulate(
			std::next(updates.begin()), updates.end(), lambda(updates.front()), 
			[lambda](std::string a, const Update& b) { return a + ", " + lambda(b); }
	);
}

// ***** FUNCTION-CALLER ***** // 

// Consume function
bool CItem::callFunction(CPlayer* p) {
  if (m_functions.count(category_) == 0) {
    p->appendPrint(use_description_->print(true));
    return false;
  }
  else
    (this->*m_functions[category_])(p);
  return true;
}

// ***** CONSUME FUNCTIONS ***** //
void CItem::consume(CPlayer* p) {
  PrintUseAndDoUpdate("consumed_item", p);
  p->getInventory().removeItemByID(id_);
}

// ***** EQUIPE-FUNCTIONS ***** //
void CItem::equipe(CPlayer* p) {
	std::cout << "CItem::equipe" << std::endl;
  p->equipeItem(this, kind_);
	std::cout << "Done" << std::endl;
}

// ***** READ-FUNCTIONS ***** //
void CItem::read(CPlayer* p) {
  PrintUseAndDoUpdate("opened_book", p);
  p->appendPrint(m_pages->pagePrint(m_mark) + "\n");
  p->addReadContext(id_);
}

void CItem::PrintUseAndDoUpdate(std::string default_use, CPlayer* p) {
  std::cout << "PrintUseAndDoUpdate" << std::endl;
  // Apply updates and print results
  std::string txt;
  p->Update(updates_, txt);
  p->appendPrint(txt);
  std::cout << "PrintUseAndDoUpdate: printing use desc" << std::endl;
  // Print default use string or use-description
  if (use_description_ == nullptr)
    p->appendDescPrint(p->getWorld()->GetSTDText(default_use));
  else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull" 
    p->appendPrint(use_description_->print(true));
#pragma GCC diagnostic pop
  }
}
