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
#include "game/config/help.h"
#include "game/config/item_config.h"
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
		ItemConfig item_config_;
		HelpConfig help_config_;
    std::map<std::string, std::string> media_;
    std::string m_path_to_world;
		bool muliplayer_;
public:

    CWorld(CPlayer*, std::string path);
    ~CWorld();
    
    // *** GETTER *** //

    ///Return path to correct world
    std::string getPathToWorld();

    ///Return json for configuration
    nlohmann::json& getConfig();

		///Return AttributeConfig 
		const AttributeConfig& attribute_config();
		AttributeConfig& attribute_config_NON_CONST();

		// Return ItemConfig 
		const ItemConfig& item_config();
		const HelpConfig& help_config();

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

    ///Return dictionary of all texts.
    map<string, nlohmann::json> getTexts(std::string area="");
		
    /// Return dictionary of all items.
    const map<string, nlohmann::json>& items();


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

		/// Creates a new detail at a given location (room).
    CDetail* GetNewDetail(std::string id, std::string room_id, CPlayer* p);
		/// Creates a new char at a given location (room).
    CPerson* GetNewChar(std::string id, std::string room_id, CPlayer* p);


    /**
    * Return a item. Look for given item in dictionary of items (jsons) and create item from json.
    * Return null-pointer if item-json couldn't be found and print error message!
    * @param[in] sID id to search item.
    * @param[in] p instance of player needed to create an object.
    * @return The created item as a pointer.
    */
    CItem* getItem(string sID, CPlayer* p);

		bool muliplayer() const;

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
		 * Reads item-config
		 * @param[in] path
		 */
    void configItemFactory(const std::string path);

		/**
		 * Reads help-config
		 * @param[in] path
		 */
    void configHelpFactory(const std::string path);


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
    map<string, CItem*> parseRoomItems(nlohmann::json j_room, CPlayer* p);

    //Characters
    void characterFactory();
    void characterFactory(std::string sPath);
    std::map<std::string, CPerson*> parseRoomChars(nlohmann::json j_room, CPlayer* p);
		/**
		 * Creates char and dialog and items belonging to char.
		 */
		CPerson* CreateCharacter(const nlohmann::json& char_json, const std::string& char_id, CPlayer* p);



    //Dialogs
    void dialogFactory(CPlayer* p);
    CDialog* dialogFactory(nlohmann::json j_states, std::string sFileName, CPlayer* p);

    //Details
    void detailFactory();
    void detailFactory(std::string sPath);
    map<string, CDetail*> parseRoomDetails(nlohmann::json j_room, CPlayer* p);
		/**
		 * Create details and items belonging to detail
		 */
		CDetail* CreateDetail(const nlohmann::json& detail_json, CPlayer* p);
    void parseRandomItemsToDetail(nlohmann::json& j_detail);


		// Tempaltes
		
		/** 
		 *  Gets the object-template (item, detail, char), updates the id to include the location_id (room, detail, or
		 *  char) and adds the extra json passed.
		 */
		std::pair<nlohmann::json, std::string> GetUpdatedTemplate(std::string location_id, std::string id, nlohmann::json j,
				const std::map<std::string, nlohmann::json>& template_map, const map<string, std::string>& existing_objs);
    void TemplateFactory(std::string path, std::map<std::string, nlohmann::json>& template_map);

    //Texts
    void textFactory();
    void textFactory(std::string);
};
