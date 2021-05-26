#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stdio.h>
#include <filesystem>
#include <streambuf>

#include <nlohmann/json.hpp>

#include "eventmanager/context.h"
#include "objects/room.h"
#include "objects/detail.h"
#include "objects/person.h"
#include "concepts/quest.h"
#include "objects/attack.h"
#include "actions/fight.h"
#include "actions/dialog.h"
#include "tools/fuzzy.h"
#include "tools/func.h"

//#include <Python.h>


using std::string;
using std::map;
using std::vector;

class CPlayer;

class CWorld
{
private:
    map<string, CRoom*>     m_rooms;
    map<string, CPerson*>   m_characters;
    map<string, CAttack*>   m_attacks;
    map<string, CQuest*>    m_quests;
    map<string, CDialog*>   m_dialogs;
    map<string, nlohmann::json> m_items;
    map<string, nlohmann::json> m_details;
    map<string, nlohmann::json> m_jCharacters;
    map<string, nlohmann::json> m_jTexts;

    map<string, vector<CText*>> m_defaultDescriptions;
    map<string, vector<CDialog*>> m_defaultDialogs;

    nlohmann::json m_config;
    std::map<std::string, std::string> media_;
    std::string m_path_to_world;
public:

    CWorld(std::string path);
    CWorld(CPlayer*, std::string path);
    
    // *** GETTER *** //

    ///Return path to correct world
    std::string getPathToWorld();

    ///Return json for configuration
    nlohmann::json& getConfig();

    ///Return standard-media (images, audio-files etc.)
    std::string media(std::string type) const;
    
    ///Return dictionary of all rooms in the game.
    map<string, CRoom*>& getRooms();

    ///Get a Room from world
    CRoom* getRoom(std::string sID);

    ///Return dictionary of all characters aka people in the game.
    map<string, CPerson*>& getCharacters();

    ///Get a character from world.
    CPerson* getCharacter(std::string sID);

    ///Return dictionary if all texts.
    map<string, nlohmann::json>& getTexts();

    ///Return dictionary of all attacks in the game.
    map<string, CAttack*>& getAttacks();

    ///Get an attack from world.
    CAttack* getAttack(std::string sID);

    ///Return dictionary of all quests in the game.
    map<string, CQuest*>& getQuests();

    ///Get a quest from world.
    CQuest* getQuest(std::string sID);

    ///Return dictionary of all dialogs in the game.
    map<string, CDialog*>& getDialogs();
    
    ///Return a dialog from world
    CDialog* getDialog(std::string sID);

    ///Return dictionary of all defaultDescriptions in the game.
    map<string, std::vector<CText*>>& getRandomDescriptions();
    
    ///Return a description from world
    CText* getRandomDescription(std::string sID);

    ///Return dictionary of all default dialogs in the game.
    map<string, std::vector<CDialog*>>& getRandomDialogs();

    ///Return a random dialog from default dialogs
    CDialog* getRandomDialog(std::string sID);

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
    map<string, CItem*> parseRoomItems(nlohmann::json j_room, CPlayer* p);

    //Characters
    void characterFactory();
    void characterFactory(std::string sPath);
    std::map<std::string, CPerson*> parseRoomChars(nlohmann::json j_room, CPlayer* p);

    //Dialogs
    void dialogFactory(CPlayer* p);
    CDialog* dialogFactory(nlohmann::json j_states, std::string sFileName, CPlayer* p);

    //Default descriptions
    void defaultDescriptionFactory(CPlayer* p);
    void defaultDescriptionFactory(std::string sFileName, CPlayer* p);

    //Default dialogs
    void defaultDialogFactory(CPlayer* p);

    //Details
    void detailFactory();
    void detailFactory(std::string sPath);
    map<string, CDetail*> parseRoomDetails(nlohmann::json j_room, CPlayer* p);
    void parseRandomItemsToDetail(nlohmann::json& j_detail);

    //Texts
    void textFactory();
    void textFactory(std::string);
};
