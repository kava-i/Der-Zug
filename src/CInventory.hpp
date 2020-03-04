#ifndef CINVENTORY_H
#define CINVENTORY_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "CItem.hpp"
#include "func.hpp"
#include "fuzzy.hpp"

class CInventory
{
private:     

    typedef std::map<std::string, std::map<string, std::vector<CItem*>> > inventory;
    inventory m_inventory;

public:
    CInventory();

    std::string printInventory(std::string color="", int highlight=-1);
    void addItem(CItem* item);
    void removeItem(std::string sItemName);
    CItem* getItem(std::string sName); 
    CItem* getItem(int a, int b);
    CItem* getItem_byID(std::string sID);
    size_t getNumRomsInColum(size_t collum);
    bool empty();
};

#endif
