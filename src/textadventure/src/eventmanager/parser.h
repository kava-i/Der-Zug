#ifndef CPARSER_H
#define CPARSER_H

#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <regex>

#include "tools/func.h"

class CParser
{
private:
	std::vector<nlohmann::json> m_commands;
public:
    CParser(nlohmann::json commands);
    typedef std::pair<std::string, std::string> event;
    std::vector<event> parse(std::string sInput);
};

#endif 
