#ifndef CDETAILS_H
#define CDETAILS_H

#include <iostream>
#include <map>
#include <string>
#include "CObject.hpp"
#include "json.hpp"

using std::string;

class CDetail : public CObject
{
private: 
    string m_sLook;

    typedef std::map<string, string> objectmap;
    objectmap m_characters;
    objectmap m_items;

public:

    //Constructor 
    CDetail(nlohmann::json jAttributes, CPlayer* p, std::string sArea);

    // *** GETTER *** //
    string getLook();
    objectmap& getCharacters();
    objectmap& getItems();

    // *** FUNCTIONS *** //
    string look(string sWhere, string sWhat);
};

#endif
