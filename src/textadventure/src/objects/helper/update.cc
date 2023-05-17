#include "objects/helper/update.h"
#include "nlohmann/json.hpp"
#include "tools/calc_enums.h"
#include <ctime>
#include <iostream>
#include <string>

Update::Update(const nlohmann::json& json) {
	id_ = json.at("id");

	std::string mod_type = json.at("mod_type");
	if (calc::MOD_TYPE_STRING_MAPPING.count(mod_type) > 0) {
		mod_type_ = calc::MOD_TYPE_STRING_MAPPING.at(mod_type);
	}
	else 
		throw "mod_type is \"" + mod_type + "\" is incorrect!";

	value_ = json.at("value");
	tmp_ = (json.at("tmp") == 1) ? true : false;
	secs_ = json.value("secs", 60);
	steps_ = json.value("steps", 0);
	rounds_ = json.value("rounds", 1);
}

void Update::ApplyUpdate(int& value) const {
	calc::MOD_TYPE_FUNCTION_MAPPING.at(mod_type_)(value, value_);
}

std::string Update::ToString(bool calc_value_from_steps) const {
	int value = (calc_value_from_steps) 
		? (steps_ == 0) ? value_ : value_/steps_
		: value_;
	std::string str = id_ + "|" + calc::MOD_TYPE_R_STRING_MAPPING.at(mod_type_) + std::to_string(value) 
		+ "|white|" + std::to_string(steps_);
	return str;
}

Updates::Updates(const nlohmann::json& json) {
	std::cout << "Parsing updates: " << json.dump() << std::endl;
	for (const auto& it : json.get<std::vector<nlohmann::json>>()) {
		updates_.push_back(Update(it));
	}
}

const std::vector<Update>& Updates::updates() const {
	return updates_;
}

void Updates::Clear() {
	updates_.clear();
}
