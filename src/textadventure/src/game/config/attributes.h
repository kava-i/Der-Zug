#pragma once

#include "nlohmann/json_fwd.hpp"
#include <iostream>
#include <map>
#include <string>
#include <vector>

enum WoozyMethods {
	ADD,
};

enum MatchType {
	GREATER, 
	LESSER,
	EQUALS
};

struct ConfigAttribute {
	ConfigAttribute() {} 
	ConfigAttribute(const nlohmann::json& json);

	std::string id_;
	std::string name_;
	std::string category_;
	int default_value_;
};

struct AttributeMapping {
	AttributeMapping() {} 
	AttributeMapping(const nlohmann::json& json);

	MatchType match_type_;
	std::string show_;
	int value_;
};

struct AttributeConfig {
	AttributeConfig() {}
	AttributeConfig(nlohmann::json json);

	std::map<std::string, ConfigAttribute> attributes_;
	std::map<std::string, std::vector<AttributeMapping>> mapping_;
	std::vector<std::string> skillable_;
	std::vector<std::string> woozy_attributes_;
	WoozyMethods woozy_method_;
	bool level_based_;
};
