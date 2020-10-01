#ifndef CROOM_H
#define CROOM_H

#include <iostream>
#include <string>
#include <map>
#include "object.h"
#include "exit.h" 
#include "concepts/text.h"
#include "item.h"
#include "detail.h"
#include "person.h"
#include "tools/gramma.h"
#include "tools/func.h"
#include "tools/fuzzy.h"

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
    CRoom(string sArea, nlohmann::json jAtts, std::map<string, CPerson*> characters, 
        std::map<string, CItem*> items, std::map<string, CDetail*> details, CPlayer* p);

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
    string showAll(CGramma* gramma);
    string showExits(CGramma* gramma);
    string showCharacters(CGramma* gramma);
    string showItems(CGramma* gramma);
    string showDetails(CGramma*);
    string look(std::string sDetail, CGramma* gramma);
    CItem* getItem(string sPlayerChoice);
};
    
#endif
