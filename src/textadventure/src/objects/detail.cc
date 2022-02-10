#include "detail.h"


CDetail::CDetail(nlohmann::json json, CPlayer* p, std::map<std::string, CItem*> items) 
    : CObject(json, p, "detail") {
  m_sLook = json.value("look", "");
  m_items = items;
}

// *** GETTER *** //
std::string CDetail::getLook() { 
  return m_sLook; 
}

std::map<std::string, CItem*>& CDetail::getItems() { 
  return m_items; 
}

