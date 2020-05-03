#ifndef CDETAILS_H
#define CDETAILS_H

#include <iostream>
#include <map>
#include <string>
#include "CObject.hpp"
#include "json.hpp"

class CItem; 

class CDetail : public CObject
{
private: 
    std::string m_sLook;
    std::map<std::string, CItem*> m_items;

public:

    //Constructor 
    CDetail(nlohmann::json jAttributes, CPlayer* p, std::map<std::string, CItem*> items);

    // *** GETTER *** //
    std::string getLook();
    std::map<std::string, CItem*>& getItems();

    // *** FUNCTIONS *** //
    std::string look(std::string sWhere, std::string sWhat);
};

#endif
