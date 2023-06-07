#pragma once

#include "nlohmann/json_fwd.hpp"
#include <iostream>
#include <optional>
#include <map>
#include <string>
#include <vector>
#include "tools/calc_enums.h"

enum WoozyMethods {
	ADD,
};

struct ConfigAttribute {
	ConfigAttribute() {} 
	ConfigAttribute(const nlohmann::json& json);

	std::string id_;
	std::string name_;
	std::string category_;
	int default_value_;
  int lower_bound_;
  std::vector<std::string> lower_events_;
  std::vector<std::string> lower_opponent_events_;
  int upper_bound_;
  std::vector<std::string> upper_events_;
  std::vector<std::string> upper_opponent_events_;

  // methods 
  bool CheckBounds(int cur ) const;
	std::string GetExceedBoundEvents(int cur, bool player=true) const;
	bool Hidden() const;
};

struct AttributeMapping {
	AttributeMapping() {} 
	AttributeMapping(const nlohmann::json& json);

	calc::MatchType match_type_;
	std::string show_;
	int value_;

  std::optional<std::string> check_mapping(int value) const;
};

struct AttributeConfig {
	// constuctors
	AttributeConfig() {}
	AttributeConfig(nlohmann::json json);

	// methods
	std::vector<std::string> SkillableNames(bool to_lower=false) const;
	void UpdateBound(std::string attribute_id, std::string match_type, int bound, bool lower_bound);


	// members
	std::map<std::string, ConfigAttribute> attributes_;
	std::map<std::string, std::vector<AttributeMapping>> mapping_;
	std::vector<std::string> skillable_;
	std::vector<std::string> fight_infos_;
	std::vector<std::string> woozy_attributes_;
	WoozyMethods woozy_method_;
	bool level_based_;
};
