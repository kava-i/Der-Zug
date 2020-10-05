#include "detail.h"


CDetail::CDetail(nlohmann::json jAttributes, CPlayer* p, std::map<std::string, CItem*> items) : CObject{jAttributes, p}
{
    m_sLook = jAttributes.value("look", "");
    m_items = items;
}

// *** GETTER *** //
std::string CDetail::getLook() { 
    return m_sLook; 
}

std::map<std::string, CItem*>& CDetail::getItems() { 
    return m_items; 
}

