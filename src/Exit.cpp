#include "CExit.hpp"

CExit::CExit(std::string sTrarget, nlohmann::json jAtts)
{
    m_sTarget = sTrarget; 
    m_sName = jAtts["name"];
    m_sPrep = jAtts.value("prep", "");
}

std::string CExit::getTarget() { 
    return m_sTarget; 
}
std::string CExit::getName() { 
    return m_sName; 
}
std::string CExit::getPrep() { 
    return m_sPrep; 

}


