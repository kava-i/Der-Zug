#ifndef CTEXT_H
#define CTEXT_H

#include <iostream>
#include "json.hpp"

class COutput;

class CText
{
private:
    std::vector<COutput*> m_texts;

public:
    CText(std::vector<COutput*> texts);

    std::string print();
};

class COutput
{
private:
    std::string sText;
    std::string sSpeacker;
public:
    COutput(nlohmann::json jAttributes);
};

#endif
    
