#ifndef CROOM_H
#define CROOM_H

#include <iostream>
#include <string>
#include <map>
#include "CObject.hpp"
#include "CExit.hpp" 
#include "CText.hpp"
#include "CItem.hpp"
#include "CDetail.hpp"
#include "CPerson.hpp"
#include "gramma.hpp"
#include "func.hpp"
#include "fuzzy.hpp"

using std::string;

class CRoom : public CObject
{ 
private:
    string m_sEntry;
    string m_sArea;

    std::map<string, std::vector<string>> m_showMap;
    
    typedef std::map<string, std::string> objectmap;
    objectmap m_players;

    std::map<string, CPerson*> m_characters;
    std::map<string, CExit*> m_exits;
    std::map<string, CItem*> m_items;
    std::map<string, CDetail*> m_details;

public:
    CRoom(string sArea, nlohmann::json jAtts, std::map<string, CPerson*> characters, std::map<string, CItem*> items, std::map<string, CDetail*> details, CPlayer* p);

    // *** getter *** // 
    string getEntry();
    string getArea(); 
    std::map<string, CPerson*>& getCharacters();
    std::map<string, CExit*>& getExtits();
    objectmap getExtits2();
    std::map<string, CItem*>& getItems();
    std::map<string, CDetail*>& getDetails();
    std::vector<nlohmann::json> getHandler();

    // *** setter *** //
    void setPlayers(objectmap& m_players);
    void setShowMap(nlohmann::json j);

    // *** various functions *** //
    string showDescription(std::map<std::string, CPerson*> mapCharacters);
    string showAll(std::string sMode, CGramma* gramma);
    string showExits(std::string sMode, CGramma* gramma);
    string showCharacters(std::string sMode, CGramma* gramma);
    string showItems(std::string sMode, CGramma* gramma);
    string showDetails(std::string sMode, CGramma*);
    string look(std::string sDetail, std::string sMode, CGramma* gramma);
    CItem* getItem(string sPlayerChoice);
};
    
#endif
