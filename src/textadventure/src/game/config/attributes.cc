#include "attributes.h"
#include "nlohmann/json.hpp"
#include "tools/calc_enums.h"
#include "tools/func.h"

ConfigAttribute::ConfigAttribute(const nlohmann::json& json) {
	id_ = json.at("id");
	name_ = json.at("name");
	category_ = json.at("category");
	default_value_ = json.at("default_value");

  if (json.value("lower_bound_enable", 0) == 1) {
    lower_bound_ = json.at("lower_bound");
    lower_events_ = json.value("lower_events", std::vector<std::string>());
    lower_opponent_events_ = json.value("lower_opponent_events", std::vector<std::string>());
  }
  else 
    lower_bound_ = 0xFFFF;
  if (json.value("upper_bound_enable", 0) == 1) {
    upper_bound_ = json.at("upper_bound");
    upper_events_ = json.value("upper_events", std::vector<std::string>());
    upper_opponent_events_ = json.value("upper_opponent_events", std::vector<std::string>());
  }
  else 
    upper_bound_ = 0xFFFF;
}

bool ConfigAttribute::CheckBounds(int cur) const {
  if (lower_bound_ != 0xFFFF && cur < lower_bound_)
    return false;
  if (upper_bound_ != 0xFFFF && cur > upper_bound_)
    return false;
  return true;
}

std::string ConfigAttribute::GetExceedBoundEvents(int cur, bool player) const {
	std::cout << "GetExceedBoundEvents: " << id_ << ", " << lower_bound_ << ", " << upper_bound_ << ", " 
		<< cur << ". player?" << player << std::endl;
  if (lower_bound_ != 0xFFFF && cur <= lower_bound_) {
		if (player)
			return func::join(lower_events_, ";");
		else 
			return func::join(lower_opponent_events_, ";");
	}
	if (upper_bound_ != 0xFFFF && cur >= upper_bound_) {
		if (player)
			return func::join(upper_events_, ";");
		else 
			return func::join(upper_opponent_events_, ";");
	}
	return "";
}
bool ConfigAttribute::Hidden() const {
	return category_.front() == '_';
}

AttributeMapping::AttributeMapping(const nlohmann::json& json) {
	show_ = json.at("show");
	value_ = json.at("value");

	std::string match_type = json.at("match_type");
	if (calc::MATCH_TYPE_STRING_MAPPING.count(match_type) > 0) {
		match_type_ = calc::MATCH_TYPE_STRING_MAPPING.at(match_type);
	}
	else 
		throw "match_type is \"" + match_type + "\" is incorrect!";
}

std::optional<std::string> AttributeMapping::check_mapping(int value) const {
	if (calc::MATCH_TYPE_FUNCTION_MAPPING.at(match_type_)(value, value_))
		return show_;
	else 
    return {};
}

AttributeConfig::AttributeConfig(nlohmann::json json) {
	for (const auto& it : json.at("attributes").get<std::vector<nlohmann::json>>()) {
		attributes_[it.at("id")] = ConfigAttribute(it);
	}

	for (const auto& it : 
      json.at("mapping").get<std::map<std::string, std::vector<nlohmann::json>>>()) {
		std::vector<AttributeMapping> mapping;
		for (const auto& map : it.second)
			mapping.push_back(AttributeMapping(map));
		mapping_[it.first] = mapping;
	}

	woozy_attributes_ = json["config"].value("woozy_attributes", std::vector<std::string>());
	std::string woozy_method = json["config"].value("woozy_method", "add");
	if (woozy_method == "add") {
		woozy_method_ = WoozyMethods::ADD;
  }
	level_based_ = json.value("levelbased", false);

	std::cout << json.dump() << std::endl;
	std::vector<std::string> skillable_categories = json["config"].value("skillable", std::vector<std::string>());
	std::vector<std::string> figh_info_categories = json["config"].value("fight_infos", std::vector<std::string>());
	std::cout << "Got skillable cats: " << func::join(skillable_categories, ", ") << std::endl;
	std::cout << "Got fight-info cats: " << func::join(figh_info_categories, ", ") << std::endl;
	for (const auto& it : attributes_) {
		if (func::inArray(skillable_categories, it.second.category_))
			skillable_.push_back(it.first);
		if (func::inArray(figh_info_categories, it.second.category_))
			fight_infos_.push_back(it.first);
	}
}

std::vector<std::string> AttributeConfig::SkillableNames(bool to_lower) const {
	std::vector<std::string> vec;
	for (const auto& it : skillable_)
		if (to_lower)
			vec.push_back(func::returnToLower(attributes_.at(it).name_));
		else 
			vec.push_back(attributes_.at(it).name_);
	return vec;
}

void AttributeConfig::UpdateBound(std::string attribute_id, std::string mod_type, int bound, bool lower_bound) {
	if (attributes_.count(attribute_id) > 0 && calc::MOD_TYPE_STRING_MAPPING.count(mod_type) > 0) {
		auto mod_func = calc::MOD_TYPE_FUNCTION_MAPPING.at(calc::MOD_TYPE_STRING_MAPPING.at(mod_type));
		if (lower_bound)
			mod_func(attributes_[attribute_id].lower_bound_, bound);
		else  {
			mod_func(attributes_[attribute_id].upper_bound_, bound);
			std::cout << "UPDATED UPPER BOUND: " << attribute_id << "-upper-bound=" << attributes_[attribute_id].upper_bound_ 
				<< std::endl;
		}
	}
	else if (attributes_.count(attribute_id) > 0)
		std::cout << "AttributeConfig::UpdateBound: Attribute " << attribute_id << " not found!" << std::endl;
	else 
		std::cout << "AttributeConfig::UpdateBound: modtype " << mod_type << " unkown!" << std::endl;
}
