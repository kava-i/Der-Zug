#include "parser.h"

CParser::CParser(nlohmann::json config) {
  m_commands = config["commands"];
}

std::vector<CParser::event> CParser::parse(std::string input) {
	std::vector<std::string> commands = func::split(input, ";");
	std::vector<event> events;
	
	for (auto command : commands) {
		//"translate" command set in config file 
		bool in_command=false;
		for (auto it : m_commands) {
			std::smatch m;
			if (std::regex_match(command, m, (std::regex)it["regex"])) {
				in_command=true;
				int take = it.value("take", 1);
				if (take > 0)
					events.push_back(std::make_pair(it["id"], m[take]));
				else {
					std::string args = "";
					for (size_t i=1; i<m.size(); i++) {
						args += m[i];
						if (i != m.size()-1)
							args += "|";
					}
					events.push_back(std::make_pair(it["id"], args));
				}
			}
		}
		if (in_command==true)
			continue;

		//Iterate over command
		std::vector<std::string> vec = func::split(command, " ");
		size_t pos = command.find(" ");
		if (pos != std::string::npos) {
			std::string target = command.substr(pos+1);
			if (target.find("to ") == 0 || target.find("up ") == 0)
				target.erase(0,3);
			events.push_back(std::make_pair(command.substr(0, pos), target));
		}
		else
			events.push_back(std::make_pair(command, command));
	}
	return events;
}
