#include "CParser.hpp"

std::vector<CParser::event> CParser::parse(std::string sInput)
{
    std::vector<std::string> vCommands = func::split(sInput, ";");
    std::vector<event> events;
    
    for(auto command : vCommands)
    {
        std::vector<std::string> vec = func::split(command, " ");
        size_t pos = command.find(" ");
        if(pos != std::string::npos)
            events.push_back(std::make_pair(command.substr(0, pos), command.substr(pos+1)));
        else
            events.push_back(std::make_pair(command, command));
    }

    return events;
}


