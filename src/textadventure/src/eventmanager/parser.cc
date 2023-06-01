#include "parser.h"

CParser::CParser(nlohmann::json commands)
{
    m_commands = commands;
}

std::vector<CParser::event> CParser::parse(std::string sInput)
{
    std::vector<std::string> vCommands = func::split(sInput, ";");
    std::vector<event> events;
    
    for(auto command : vCommands)
    {
        //"translate" command set in config file 
        bool in_command=false;
        for(auto it : m_commands["commands"]) {
            std::smatch m;
            if(std::regex_match(command, m, (std::regex)it["regex"])) {
                in_command=true;
								int take = it.value("take", 1);
								if (take > 0)
                	events.push_back(std::make_pair(it["id"], m[take]));
								else {
									std::string args = "";
									for (int i=1; i<m.size(); i++) {
										args += m[i];
										if (i != m.size()-1)
											args += "|";
									}
                	events.push_back(std::make_pair(it["id"], args));
								}
            }
        }
        if(in_command==true)
            continue;

        //Iterate over command
        std::vector<std::string> vec = func::split(command, " ");
        size_t pos = command.find(" ");
        if(pos != std::string::npos)
        {
            std::string sSecond = command.substr(pos+1);
            if(sSecond.find("to ") == 0 || sSecond.find("up ") == 0)
                sSecond.erase(0,3);

            events.push_back(std::make_pair(command.substr(0, pos), sSecond));
        }
        else
            events.push_back(std::make_pair(command, command));
    }
    return events;
}
