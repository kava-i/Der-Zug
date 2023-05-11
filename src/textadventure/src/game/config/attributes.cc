#include "attributes.h"
#include "nlohmann/json.hpp"

ConfigAttribute::ConfigAttribute(const nlohmann::json& json) {
	id_ = json.at("id");
	name_ = json.at("name");
	category_ = json.at("category");
	default_value_ = json.at("default_value");
}

AttributeMapping::AttributeMapping(const nlohmann::json& json) {
	show_ = json.at("show");
	value_ = json.at("value");

	std::string match_type = json.at("match_type");
	if (match_type == ">") 
		match_type_ = MatchType::GREATER;
	else if (match_type == "<") 
		match_type_ = MatchType::LESSER;
	else if (match_type == "=") 
		match_type_ = MatchType::EQUALS;
	else 
		throw "match_type is neight \">\", \"<\" or \"=\"";
}

AttributeConfig::AttributeConfig(nlohmann::json json) {
	for (const auto& it : json.at("attributes").get<std::vector<nlohmann::json>>()) {
		attributes_[it.at("id")] = ConfigAttribute(it);
	}

	for (const auto& it : json.at("mapping").get<std::map<std::string, std::vector<nlohmann::json>>>()) {
		std::vector<AttributeMapping> mapping;
		for (const auto& map : it.second)
			mapping.push_back(AttributeMapping(map));
		mapping_[it.first] = mapping;
	}

	skillable_ = json.value("skillable", std::vector<std::string>());
	woozy_attributes_ = json.value("woozy_attributes", std::vector<std::string>());
	std::string woozy_method = json.value("woozy_method", "add");
	if (woozy_method == "add") 
		woozy_method_ = WoozyMethods::ADD;

	level_based_ = json.value("levelbased", false);
}
