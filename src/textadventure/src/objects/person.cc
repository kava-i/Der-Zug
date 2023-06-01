#include "person.h"
#include "game/config/attributes.h"
#include "player.h"
#include "tools/fuzzy.h"
#include "tools/webcmd.h"

#define GREEN Webcmd::set_color(Webcmd::color::GREEN)
#define RED Webcmd::set_color(Webcmd::color::RED)
#define WHITE Webcmd::set_color(Webcmd::color::WHITE)


/**
* Constructor for Person, i.e. "character". Calls constructor from base class CObject.
* param[in] jAtts json with attributes
* param[in] dialogue dialogue of this character.
* param[in] items list of items 
* param[in] attacks list of attacks
*/
CPerson::CPerson(nlohmann::json jAttributes, CDialog* dialogue, attacks newAttacks, CText* text, 
    CPlayer* p, std::map<std::string, CDialog*> dialogs, const std::map<std::string, ConfigAttribute>& all_attributes,
		t_map_items items) 
    : CObject(jAttributes, p, "person"), updates_(jAttributes.value("updates", nlohmann::json::array())) {
  // Set stats, then update missing stats with default value
	attributes_ = jAttributes.value("attributes", std::map<std::string, int>());
	for (const auto& it : all_attributes) {
		if (attributes_.count(it.first) == 0)
			attributes_[it.first] = it.second.default_value_;
	}

	color_ = Webcmd::COLOR_MAPPING.at(jAttributes.value("color", "white"));

  will_faint_ = (bool)jAttributes.value("faint", 0);
	fainted_ = false;
  
  m_roomDescription = new CText(jAttributes.value("roomDescription", nlohmann::json::parse("{}")), p);
  m_deadDescription = jAttributes.value("deadDescription", nlohmann::json::parse("{}"));

  if (text!=nullptr)
    text_ = text;

  //Set dialogue and attacks
  m_attacks = newAttacks;
  m_dialog = dialogue;
  m_dialogs = dialogs;

  //Add items to inventory one by one
  for (auto &item : items)
    m_inventory.addItem(item.second);
}

// *** GETTER *** //

///return map of stats
std::map<std::string, int>& CPerson::getStats() {
  return attributes_;
}

/**
* Return person's stats, like strength, virtue, live-points, by passing name of stat.
* @param[in] id specify which stat shall be returned.
* @return return given stat.
*/
int CPerson::getStat(std::string id, int default_value) { 
	try {
    return attributes_.at(id); 
	}
	catch (std::exception& e) {
  	std::cout << "Attribute accessed which does not exits: " << id << std::endl;
  	return default_value;
	}
}

/// return whether character can faint or dies immediately 
bool CPerson::will_faint() {
  return will_faint_;
}

/// return whether character can faint or dies immediately 
bool CPerson::fainted() {
  return fainted_;
}

std::string CPerson::color() const {
	return color_;

}
///Return person's dialogue.
CDialog* CPerson::getDialog() { 
  return m_dialog; 
}

CDialog* CPerson::getDialog(std::string dialog) {
  if (m_dialogs.count(dialog) > 0)
    return m_dialogs[dialog];
  std::cout << "Dialog not found: " << dialog << ".\n";
  return nullptr;
}

///Return person's attacks.
CPerson::attacks& CPerson::getAttacks() { 
  return m_attacks; 
}

std::string CPerson::getAttacksID(std::string name) {
	std::string id = "";
	double min = 1;
	for (const auto& it : m_attacks) {
		auto res = fuzzy::fuzzy_cmp(it.second->getName(), name);
		if (res <= 0.2 && res < min) {
			id = it.first;
			min = res;
		}
	}
	return id;
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
  return text_->reducedPrint(false);
}

///Get description of character, when he is dead.
nlohmann::json CPerson::getDeadDescription() {
  return m_deadDescription;
}

const Updates& CPerson::updates() {
	return updates_;
}

// *** SETTER *** //

///Set a new stat of this person
void CPerson::setStat(std::string id, int stat) {
  attributes_[id] = stat;
}

///Set person's dialogue.
void CPerson::setDialog(CDialog* newDialog) { 
  m_dialog = newDialog; 
}

///Set person's dialogue, taking a dialog from persons map of dialogues.
void CPerson::setDialog(std::string dialog) {
  if (m_dialogs.count(dialog) > 0)
    m_dialog = m_dialogs[dialog];
  else {
    std::cout << "Dialog not found: " << dialog << ".\n";
		for (const auto& it : m_dialogs) 
			std::cout << " - " << it.first << std::endl;
	}
}

void CPerson::setFainted(bool fainted) {
	fainted_ = fainted;
}

// *** ATTACKS *** //

/**
* Print all attacks. Attacks are printed in the form: Name \n Strengt\n Description.
*/
string CPerson::printAttacks() {
  string attacks_print = "Attacks: \n";
  //Iterate over attacks and add to output.
	for (const auto& it : m_attacks) {
		attacks_print += "- " + it.second->getName() + ": <i>" + it.second->getDescription() + "</i>\n";
	}
  return attacks_print;
}

string CPerson::printFightInfos(const AttributeConfig& attribute_conf) const {
	std::string infos;
	for (const auto& it : attribute_conf.fight_infos_) {
		if (attributes_.count(it) > 0) 
			infos += attribute_conf.attributes_.at(it).name_ + ": " + std::to_string(attributes_.at(it)) + ", ";
	}
	if (infos.size() > 2)
		return infos.substr(0, infos.size()-2);
	return infos;
}

string CPerson::getAttack(string sPlayerChoice) {
  auto lambda = [](CAttack* attack) { return attack->getName(); };
  std::string sAttack = func::getObjectId(m_attacks, sPlayerChoice, lambda);
  if (m_attacks.count(sAttack) > 0)
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
  return attributes_.count(func::returnToLower(sAttribute)) > 0;
}

std::string CPerson::getAllInformation() {
  std::string sOutput = "";
  sOutput += "id: " + id_ + ", name: " + name_ + ", ";
  for(auto it : attributes_)
    sOutput += it.first + ": " + std::to_string(it.second) + ", ";
  return sOutput;
}

std::string CPerson::SimpleUpdate(const Updates& updates, std::string& msg, 
		const std::map<std::string, ConfigAttribute>& conf_attributes, bool player) {
	std::cout << "SimpleUpdate called from player? " << player << std::endl;
	std::string rtn_events = "";
	for (const auto& it : updates.updates()) {
		auto value = it.value_ * ((attributes_.count(it.attribute_) > 0) ? attributes_.at(it.attribute_) : 1);
    std::string opt = calc::MOD_TYPE_MSG_MAPPING.at(it.mod_type_);
		if (attributes_.count(it.id_) > 0) {
      int val_old = attributes_.at(it.id_);
			it.ApplyUpdate(attributes_.at(it.id_), value);
			std::string name = conf_attributes.at(it.id_).name_;
      std::string color = (val_old <= attributes_.at(it.id_)) ? GREEN : RED;
			if (conf_attributes.at(it.id_).category_.front() != '_')
      	msg += color + name + opt + func::dtos(value) + WHITE + "\n";
			auto events = conf_attributes.at(it.id_).GetExceedBoundEvents(attributes_.at(it.id_), player);
			if (events.size() > 0)
				rtn_events += (rtn_events.length() == 0) ? events : ";" + events;
		}
	}
	return rtn_events;
}
