#include "objects/helper/update.h"
#include "nlohmann/json.hpp"
#include "tools/calc_enums.h"
#include <ctime>
#include <iostream>
#include <string>

Update::Update(const nlohmann::json& json) {
	id_ = json.at("id");

	std::string mod_type = json.at("mod_type");
	if (calc::MOD_TYPE_STRING_MAPPING.count(mod_type) > 0)
		mod_type_ = calc::MOD_TYPE_STRING_MAPPING.at(mod_type);
	else 
		throw "mod_type is \"" + mod_type + "\" is incorrect!";

	value_ = json.at("value");
  attribute_ = json.value("attribute", "");
	tmp_ = (json.at("tmp") == 1) ? true : false;
	secs_ = json.value("secs", 60);
	steps_ = json.value("steps", 1);
	rounds_ = json.value("rounds", 1);
}

Update::Update(const Update& update) : id_(update.id_), mod_type_(update.mod_type_),
  value_(update.value_), attribute_(update.attribute_), tmp_(update.tmp_),
  secs_(update.secs_), steps_(update.steps_), rounds_(update.rounds_) {}

Update::Update(std::string attribute, double value, calc::ModType mod_type) 
  : id_(attribute), mod_type_(mod_type), value_(value), attribute_(""), 
  tmp_(false), secs_(60), steps_(1), rounds_(1) {}

void Update::ApplyUpdate(int& value, int use_value) const {
	calc::MOD_TYPE_FUNCTION_MAPPING.at(mod_type_)(value, use_value);
}

std::string Update::ToString(bool calc_value_from_steps, bool reverse) const {
	int value = (!calc_value_from_steps) ? value_ : value_/steps_;
  calc::ModType mt = (reverse) ? calc::MOD_TYPE_R_MAPPING.at(mod_type_) : mod_type_;
	std::string str = id_ + "|" + calc::MOD_TYPE_R_STRING_MAPPING.at(mt) + "|" 
    + std::to_string(value) + "*" + attribute_ + "|white|" + std::to_string(steps_);
	return str;
}

void Update::ReverseModType() {
  mod_type_ = calc::MOD_TYPE_R_MAPPING.at(mod_type_);
}

// **** UPDATES **** //

Updates::Updates(const nlohmann::json& json) {
	std::cout << "Parsing updates: " << json.dump() << std::endl;
	for (const auto& it : json.get<std::vector<nlohmann::json>>())
		updates_.push_back(Update(it));
}

Updates::Updates(const std::vector<Update>& updates) {
  for (const auto& it : updates) 
    updates_.push_back(it);
}

Updates::Updates(std::map<std::string, int> attributes, calc::ModType mod_type) {
  for (const auto& it : attributes)
		updates_.push_back(Update(it.first, it.second, mod_type));
}

const std::vector<Update>& Updates::updates() const {
	return updates_;
}

void Updates::Clear() {
	updates_.clear();
}

Updates Updates::Reverse() const {
	std::vector<Update> updates;
  for (const auto& it : updates_) {
    updates.push_back(Update(it));
    updates.back().ReverseModType();
  }
  return updates;
}
