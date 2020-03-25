#ifndef CROOM_H
#define CROOM_H

#include <iostream>
#include <string>
#include <map>
#include "CObject.hpp"
#include "CExit.hpp"
#include "CText.hpp"
#include "CCharacter.hpp"
#include "CItem.hpp"
#include "CDetail.hpp"
#include "fuzzy.hpp"

using std::string;

class CRoom : public CObject
{ 
private:
    CText* m_text;
    string m_sEntry;
    string m_sArea;
    
    typedef std::map<string, std::string> objectmap;
    objectmap m_characters;
    objectmap m_exits_objectMap;
    objectmap m_players;

    std::map<string, CExit*> m_exits;
    std::map<string, CItem*> m_items;
    std::map<string, CDetail*> m_details;

public:
    CRoom(string sArea, nlohmann::json jAtts, CText* text, objectmap characters, std::map<string, CItem*> items, std::map<string, CDetail*> details) : CObject{jAtts}
    {
        m_sArea = sArea;
        m_sEntry = jAtts.value("entry", "");

        std::map<std::string, nlohmann::json> mapExits = jAtts["exits"].get<std::map<std::string, nlohmann::json>>();
        for(const auto &it : mapExits) 
            m_exits[it.first] = new CExit(it.first, it.second);
        auto lamda = [](CExit* exit) { return exit->getName(); };
        m_exits_objectMap = func::convertToObjectmap(m_exits, lamda);

        m_text = text;
        m_characters = characters;
        m_items = items;
        m_details = details; 
    }

    // *** getter *** // 
    string getDescription();
    string getEntry();
    string getArea(); 
    objectmap& getCharacters();
    objectmap& getExtits2();
    std::map<string, CExit*>& getExtits();
    std::map<string, CItem*>& getItems();

    // *** setter *** //
    void setPlayers(objectmap& m_players);

    // *** various functions *** //
    string showEntryDescription(std::map<string, CCharacter*>& mapChars);
    string showDescription(std::map<string, CCharacter*>& mapChars); 
    string showAll(std::string sMode);
    string showExits(std::string sMode);
    string showCharacters(std::string sMode);
    string showItems(std::string sMode);
    string showDetails(std::string sMode);
    string look(string sWhere, string sWhat, std::string sMode);
    CItem* getItem(string sPlayerChoice);
};
    
#endif
