#ifndef CEXIT_H
#define CEXIT_H 

#include <iostream>
#include <string>
#include "CObject.hpp"
#include "json.hpp"

class CExit : public CObject
{ 
private:
    std::string m_sTarget; 
    std::string m_sPrep;

public:
    CExit(std::string sTrarget, nlohmann::json jAtts, CPlayer* p) : CObject{jAtts, p}
    {
        m_sTarget = sTrarget;
        m_sPrep = jAtts.value("prep", "");
    }

    std::string getTarget();
    std::string getPrep();
};

#endif 
