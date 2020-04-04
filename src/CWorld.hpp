#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include <experimental/filesystem>
#include <streambuf>
#include "CEnhancedContext.hpp"
#include "CRoom.hpp"
#include "CDetail.hpp"
#include "CPerson.hpp"
#include "CQuest.hpp"
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

class CPlayer;

class CWorld
{
private:
    map<string, CRoom*>      m_rooms;
    map<string, CPerson*> m_characters;
    map<string, CAttack*>    m_attacks;
    map<string, CQuest*>     m_quests;
    map<string, nlohmann::json> m_items;

public:

    CWorld();
    CWorld(CPlayer*);
    
    // *** GETTER *** //
    
    ///Return dictionary of all rooms in the game.
    map<string, CRoom*>& getRooms();

    ///Return dictionary of all characters aka people in the game.
    map<string, CPerson*>& getCharacters();

    ///Return dictionary of all attacks in the game.
    map<string, CAttack*>& getAttacks();

    ///Return dictionary of all quests in the game.
    map<string, CQuest*>& getQuests();

    /**
    * Return a item. Look for given item in dictionary of items (jsons) and create item from json.
    * Return null-pointer if item-json couldn't be found and print error message!
    * @param[in] sID id to search item.
    * @param[in] p instance of player needed to create an object.
    * @return The created item as a pointer.
    */
    CItem* getItem(string sID, CPlayer* p);

    // *** FACTORYS *** // 
    typedef map<string, string> objectmap;
    void worldFactory(CPlayer* p);

    //Room
    void roomFactory(CPlayer* p);
    void roomFactory(string sPath, CPlayer* p);

    //Attacks
    void attackFactory();
    void attackFactory(std::string sPath);
    map<string, CAttack*> parsePersonAttacks(nlohmann::json j_person);
    
    //Quests
    void questFactory();
    void questFactory(std::string sPath);

    //Items
    void itemFactory();
    void itemFactory(std::string sPath);
    map<string, CItem*> parseRoomItems(nlohmann::json j_room, std::string sArea, CPlayer* p);

    //Character, Dialog, Details
    map<string, CDetail*> detailFactory(nlohmann::json j_room, CPlayer* p, std::string sArea);
    objectmap characterFactory(nlohmann::json j_characters, CPlayer* p);
    SDialog* dialogFactory(string sPath, CPlayer* p); 
};
