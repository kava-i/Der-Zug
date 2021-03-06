#include "inventory.h"

CInventory::CInventory() {}


std::string CInventory::printInventory(std::string color, int highlight)
{
    std::string sOutput;

    if(m_inventory.size() == 0)
        return sOutput + "Inventar leer.\n";

    auto lamda1 = [](std::string x, std::vector<CItem*> vec) { 
                    return std::to_string(vec.size()) + "x " + x; };
    auto lamda2 = [](std::string x, std::vector<CItem*> vec) { 
                    return std::to_string(vec[0]->getValue()); };

    sOutput += func::table(m_inventory, lamda1, lamda2, "width:auto;border:1px solid black", highlight); 
    return sOutput;
}

void CInventory::addItem(CItem* item)
{
    m_inventory[item->getCategory()][item->getName()].push_back(item); 
}

void CInventory::removeItemByID(std::string sItemID)
{
    for(auto &it : m_inventory) {
        for(auto jt = it.second.begin(); jt != it.second.end(); ++jt) {
            for(auto it2 = (*jt).second.begin(); it2 != (*jt).second.end(); ++it2)
            {
                if((*it2)->getID() == sItemID)
                {
                    if((*jt).second.size() == 1)
                    {
                        (*jt).second.clear();
                        it.second.erase(jt);
                    }
                    else
                        it2 = (*jt).second.erase(it2);
                    return;
                }
            }
        }
    }
}

CItem* CInventory::getItem(std::string sName)
{
    for(auto& it : m_inventory) {
        for(auto& jt : it.second) {
            if(fuzzy::fuzzy_cmp(jt.second[0]->getName(), sName) <= 0.2)
                return jt.second.back();
        }
    }
    return NULL;
}


CItem* CInventory::getItem(int a, int b)
{
    auto it = m_inventory.begin();
    std::advance(it, a);
    auto jt = it->second.begin();
    std::advance(jt, b);
    return jt->second.back();
}

CItem* CInventory::getItem_byID(std::string sID)
{
    for(auto it : m_inventory) {
        for(auto jt : it.second) {
	        for(auto kl : jt.second ) {
		        if(kl->getID() == sID)
		            return kl;
	        }
        }
    }
    return NULL;
}

size_t CInventory::getNumRomsInColum(size_t colum)
{
    auto it = m_inventory.begin();
    std::advance(it, colum);
    return it->second.size();
}

bool CInventory::empty()
{
    if(m_inventory.size() == 0)
        return true;

    bool empty = true;
    for (auto it : m_inventory) {
        if(it.second.size() > 0)
            empty = false;
    } 
    return empty;
}


std::string CInventory::getItemList() {
  std::string items = "";
  for (auto it : mapItems())
    items+=it.first + ";";
  return items.substr(0, items.length()-1);
}

std::map<std::string, CItem*> CInventory::mapItems()
{
    std::map<std::string, CItem*> mapItems;
    for(auto it : m_inventory)
    {
        for(auto jt : it.second)
        {
            for(auto kt : jt.second)
                mapItems[kt->getID()] = kt;
        }
    }
    return mapItems;
}
