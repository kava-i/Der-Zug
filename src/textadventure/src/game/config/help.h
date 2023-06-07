#pragma once

#include "nlohmann/json_fwd.hpp"
#include <iostream>

struct HelpEntry {
	std::string id_;
	std::string text_;
	bool active_;

	HelpEntry() {}
	HelpEntry(const nlohmann::json& j);
};

struct HelpConfig {
	std::map<std::string, std::vector<HelpEntry>> help_;

	HelpConfig() {};
	HelpConfig(const nlohmann::json& j);

	const std::vector<HelpEntry>& GetHelp(const std::string& context) const;
	void UpdateActive(const std::string& context, const std::string& id, bool active);
};
