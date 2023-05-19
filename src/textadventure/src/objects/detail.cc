#include "detail.h"


CDetail::CDetail(nlohmann::json json, CPlayer* p, std::map<std::string, CItem*> items) 
    : CObject(json, p, "detail") {
  m_sLook = json.value("look", "");
  m_items = items;
	std::cout << "Added detail from json: " << json.dump() << std::endl;
	std::cout << "id: " << id_ << std::endl;
	std::cout << "id: " << name_ << std::endl;
}

// *** GETTER *** //
std::string CDetail::getLook() { 
  return m_sLook; 
}

std::map<std::string, CItem*>& CDetail::getItems() { 
  return m_items; 
}

