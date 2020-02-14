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
        {
            std::string sSecond = command.substr(pos+1);
            //Do some preprocessing
            if(sSecond.find("to ") == 0 || sSecond.find("up ") == 0)
                sSecond.erase(0,3);

            events.push_back(std::make_pair(command.substr(0, pos), sSecond));
        }
        else
            events.push_back(std::make_pair(command, command));
    }

    return events;
}

