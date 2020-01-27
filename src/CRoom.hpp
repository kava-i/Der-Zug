#ifndef CROOM_H
#define CROOM_H

#include <iostream>
#include <string>
#include <map>
#include "CCharacter.hpp"
#include "CItem.hpp"
#include "CDetail.hpp"
#include "fuzzy.hpp"

using std::string;

class CRoom
{ 
private:
    string m_sName;
    string m_sID;
    string m_sDescription;
    string m_sEntry;
    
    typedef std::map<string, std::string> objectmap;
    objectmap m_exists;
    objectmap m_characters;
    objectmap m_players;

    std::map<string, CItem*> m_items;
    std::map<string, CDetail*> m_details;

public:
    CRoom(string sName, string sID, string sDescription, string sEntry, objectmap exits, objectmap characters, std::map<string, CItem*> items, std::map<string, CDetail*> details);

    // *** getter *** // 
    string getName();
    string getID();
    string getDescription();
    string getEntry();
    objectmap& getExtits();
    objectmap& getCharacters();
    std::map<string, CItem*>& getItems();

    // *** setter *** //
    void setPlayers(objectmap& m_players);

    // *** various functions *** //
    string showEntryDescription(std::map<string, CCharacter*>& mapChars);
    string showDescription(std::map<string, CCharacter*>& mapChars); 
    string showAll();
    string showExits();
    string showCharacters();
    string showItems();
    string showDetails();
    string look(string sWhere, string sWhat);
    CItem* getItem(string sPlayerChoice);
};
    
#endif
