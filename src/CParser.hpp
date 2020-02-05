#ifndef CPARSER_H
#define CPARSER_H

#include <iostream>
#include "func.hpp"

class CParser
{
public:
    typedef std::pair<std::string, std::string> event;
    std::vector<event> parse(std::string sInput);
};

#endif 
