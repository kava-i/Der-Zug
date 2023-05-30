#pragma once 

#include "nlohmann/json.hpp"
#include <string>
#include <vector>

struct ItemConfig {
	std::map<std::string, std::vector<std::string>> _kinds;

	ItemConfig() {}
	ItemConfig(const nlohmann::json& j) {
		_kinds = j.value("kinds", std::map<std::string, std::vector<std::string>>());
	}
};
