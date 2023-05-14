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
#include "game/config/attributes.h"
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
    map<string, std::pair<std::string, nlohmann::json>> m_jTexts;

    map<string, vector<CText*>> m_defaultDescriptions;

    nlohmann::json m_config;
		AttributeConfig attribute_config_;
    std::map<std::string, std::string> media_;
    std::string m_path_to_world;
public:

    CWorld(CPlayer*, std::string path);
    
    // *** GETTER *** //

    ///Return path to correct world
    std::string getPathToWorld();

    ///Return json for configuration
    nlohmann::json& getConfig();

		///Return AttributeConfig 
		const AttributeConfig& attribute_config();

    std::string GetSTDText(std::string txt);

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
    map<string, nlohmann::json> getTexts(std::string area="");

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

    ///Return a description from world
    CText* getRandomDescription(std::string sID, CPlayer* p);

    CDetail* GetDetail(std::string id, std::string room_id, CPlayer* p);

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

    // config 
    /**
     * Reads config and checks that all neccesary fields are included.
     * @param[in] path
     */
    void configFactory(const std::string path);

    /**
     * Reads attribute-config 
     * @param[in] path
     */
    void configAttributeFactory(const std::string path);


    /** 
     * Extracs and then sets media data (image, sound) from config.
     */
    void configExtractor(CPlayer* p);

    //Room
    void roomFactory(CPlayer* p);
    void roomFactory(string sPath, CPlayer* p);

    //Attacks
    void attackFactory();
    void attackFactory(std::string sPath);
    map<string, CAttack*> parsePersonAttacks(std::vector<std::string> attack_ids);
    
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

    //Details
    void detailFactory();
    void detailFactory(std::string sPath);
    map<string, CDetail*> parseRoomDetails(nlohmann::json j_room, CPlayer* p);
    void parseRandomItemsToDetail(nlohmann::json& j_detail);

    //Texts
    void textFactory();
    void textFactory(std::string);
};
