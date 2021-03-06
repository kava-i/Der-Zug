#ifndef CINVENTORY_H
#define CINVENTORY_H

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "objects/item.h"
#include "tools/func.h"
#include "tools/fuzzy.h"

class CInventory
{
private:     

    typedef std::map<std::string, std::map<std::string, std::vector<CItem*>> > inventory;
    inventory m_inventory;

public:
    CInventory();

    std::string printInventory(std::string color="", int highlight=-1);
    void addItem(CItem* item);
    void removeItemByID(std::string sItemName);
    CItem* getItem(std::string sName); 
    CItem* getItem(int a, int b);
    CItem* getItem_byID(std::string sID);
    size_t getNumRomsInColum(size_t collum);
    bool empty();

    std::map<std::string, CItem*> mapItems();
    std::string getItemList();
};

#endif
