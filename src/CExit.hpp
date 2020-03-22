#ifndef CEXIT_H
#define CEXIT_H 

#include <iostream>
#include <string>
#include "json.hpp"

class CExit
{ 
private:
    std::string m_sTarget; 
    std::string m_sName;
    std::string m_sPrep;

public:
    CExit(std::string sTrarget, nlohmann::json jAtts);

    std::string getTarget();
    std::string getName();
    std::string getPrep();
};

#endif 
