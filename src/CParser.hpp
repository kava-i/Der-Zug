#ifndef CPARSER_H
#define CPARSER_H

#include <iostream>
#include <map>
#include <string>
#include <regex>
#include "func.hpp"
#include "json.hpp"

class CParser
{
private:
    nlohmann::json m_commands;
public:
    CParser(nlohmann::json commands);
    typedef std::pair<std::string, std::string> event;
    std::vector<event> parse(std::string sInput);
};

#endif 
