#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include <experimental/filesystem>
#include <streambuf>
#include "CRoom.hpp"
#include "CDetail.hpp"
#include "CPerson.hpp"
#include "CCharacter.hpp"
#include "CAttack.hpp"
#include "CFight.hpp"
#include "CDialog.hpp"
#include "json.hpp"
#include "fuzzy.hpp"
#include "func.hpp"

//#include <Python.h>

using std::string;
using std::map;
using std::vector;

class CWorld
{
private:
    map<string, CRoom*>      m_rooms;
    map<string, CCharacter*> m_characters;
    map<string, CAttack*>    m_attacks;
    map<string, nlohmann::json> m_items;

public:

    CWorld();
    
    // *** GETTER *** //
    map<string, CRoom*>& getRooms() { return m_rooms; }
    map<string, CCharacter*>& getCharacters() { return m_characters; }
    map<string, CAttack*>& getAttacks() {return m_attacks; }
    CItem* getItem(string sID) { return new CItem(m_items[sID]); }

    // *** FACTORYS *** // 
    typedef map<string, string> objectmap;
    void worldFactory();

    //Room
    void roomFactory();
    void roomFactory(string sPath);

    //Attacks
    void attackFactory();
    void attackFactory(std::string sPath);
    map<string, CAttack*> parsePersonAttacks(nlohmann::json j_person);

    //Items
    void itemFactory();
    void itemFactory(std::string sPath);
    map<string, CItem*> parseRoomItems(nlohmann::json j_room);

    //Character, Dialog, Details
    map<string, CDetail*>  detailFactory(nlohmann::json j_room);
    objectmap characterFactory(nlohmann::json j_characters);
    SDialog* dialogFactory(string sPath); 
};
