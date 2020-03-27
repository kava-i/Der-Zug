#include "CDetail.hpp"


CDetail::CDetail(nlohmann::json jAttributes, CPlayer* p, std::string sArea) : CObject{jAttributes, p}
{
    m_sLook = jAttributes.value("look", "in");

    if(jAttributes.count("characters") > 0)
        m_characters = jAttributes["characters"].get<objectmap>();

    if(jAttributes.count("items") > 0)
    {
        for(const auto &it : jAttributes["items"].get<objectmap>())
            m_items[sArea + "_" + it.first] = it.second;
    }
}

// *** GETTER *** //
string CDetail::getLook() { 
    return m_sLook; 
}

CDetail::objectmap& CDetail::getCharacters() { 
    return m_characters; 
}

CDetail::objectmap& CDetail::getItems() { 
    return m_items; 
}
 

// *** FUNCTIONS *** //
string CDetail::look(string sWhere, string sWhat) {
    return "";
}
