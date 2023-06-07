#include "help.h"
#include "nlohmann/json.hpp"
#include <exception>

HelpEntry::HelpEntry(const nlohmann::json& j) {
	id_ = j.at("id"); 
	text_ = j.at("txt");
	active_ = j.value("active", 1) == 1;
}

HelpConfig::HelpConfig(const nlohmann::json& j) {
	for (const auto& it : j.get<std::map<std::string, std::vector<nlohmann::json>>>()) {
		std::vector<HelpEntry> help;
		for (const auto& jt : it.second)
			help.push_back(HelpEntry(jt));
		std::cout << "Initialize HELP for " << it.first << " with " << help.size() << " entries" << std::endl;
		help_[it.first] = help;
	}
}

const std::vector<HelpEntry>& HelpConfig::GetHelp(const std::string& context) const {
	if (help_.count(context) > 0) {
		return help_.at(context);
	}
	throw std::invalid_argument("No help for this context");
}

void HelpConfig::UpdateActive(const std::string& context, const std::string& id, bool active) {
	if (help_.count(context) > 0) {
		for (auto& it : help_.at(context)) {
			if (it.id_ == id) {
				std::cout << "Set " << context << "-" << id << " to: " << active << std::endl;
				it.active_ = active;
			}
		}
	}
	else {
		std::cout << "Help-Context not found." << std::endl;

	}
}
