#include "CWorld.hpp"

#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

namespace fs = std::experimental::filesystem;

CWorld::CWorld() {
    std::ifstream readPath("factory/current_world.json");
    nlohmann::json j;
    readPath >> j;
    if(j.count("world") > 0)
        m_path_to_world = j["world"];
    else
        m_path_to_world = "factory/world1/";
    readPath.close();

    std::ifstream readConfig(m_path_to_world + "config.json");
    readConfig >> m_config;
    readConfig.close();
}

CWorld::CWorld(CPlayer* p) {
    std::ifstream readPath("factory/current_world.json");
    nlohmann::json j;
    readPath >> j;
    if(j.count("world") > 0)
        m_path_to_world = j["world"];
    else
        m_path_to_world = "factory/world1/";
    readPath.close();

    worldFactory(p);

    std::ifstream readConfig(m_path_to_world + "config.json");
    readConfig >> m_config;
    readConfig.close();
}

// *** GETTER *** // 

///Return path to correct world
std::string CWorld::getPathToWorld() {
    return m_path_to_world;
}

///Return json for configuration
nlohmann::json& CWorld::getConfig() {
    return m_config;
}

///Return dictionary of all rooms in the game.
map<string, CRoom*>& CWorld::getRooms() { 
    return m_rooms; 
}

///Get a Room from world
CRoom* CWorld::getRoom(std::string sID) {
    if(m_rooms.count(sID) > 0)
        return m_rooms[sID];
    std::cout << cRED << "FATAL! Accessing room which does not exist: " << sID << cCLEAR << std::endl;
    return nullptr;
}
     
///Return dictionary of all characters aka people in the game.
map<string, CPerson*>& CWorld::getCharacters() { 
    return m_characters; 
}

///Get a character from world.
CPerson* CWorld::getCharacter(std::string sID) {
    if(m_characters.count(sID) > 0)
        return m_characters[sID];
    std::cout<<cRED<< "FATAL! Accessing character which does not exist: " << sID <<cCLEAR<< std::endl;
    return nullptr;
}

///Return dictionary of all attacks in the game.
map<string, CAttack*>& CWorld::getAttacks() { 
    return m_attacks; 
}

///Get an attack from world.
CAttack* CWorld::getAttack(std::string sID)
{
    if(m_attacks.count(sID) > 0)
        return m_attacks[sID];
    std::cout << cRED << "FATAL! Accessing attack which does not exist: " << sID <<cCLEAR<<std::endl;
    return nullptr;
}

///Return dictionary of all quests in the game.
map<string, CQuest*>& CWorld::getQuests() { 
    return m_quests; 
}

///Get a quest from world.
CQuest* CWorld::getQuest(std::string sID)
{
    if(m_quests.count(sID) > 0)
        return m_quests[sID];
    std::cout << cRED << "FATAL!!! Accessing quest which does not exist: " << sID << cCLEAR << std::endl;
    return nullptr;
}

///Return dictionary of all dialogs in the game.
map<string, CDialog*>& CWorld::getDialogs() {
    return m_dialogs;
}

///Return a dialog from world
CDialog* CWorld::getDialog(std::string sID)
{
    if(m_dialogs.count(sID) > 0)
        return m_dialogs[sID];
    std::cout << cRED << "FATAL!!! Accessing dialog which does not exist: " << sID << cCLEAR << std::endl;
    return nullptr;
}

///Return dictionary of all default descriptions in the game.
map<string, std::vector<CText*>>& CWorld::getRandomDescriptions() {
    return m_defaultDescriptions;
}

///Return a randowm description from default descriptions
CText* CWorld::getRandomDescription(std::string sID)
{
    if(m_defaultDescriptions.count(sID) > 0) {
        size_t num = rand() % m_defaultDescriptions[sID].size();
        return m_defaultDescriptions[sID][num];
    }
    std::cout << cRED << "FATAL!!! Accessing description which does not exist: " << sID << cCLEAR << std::endl;
    return nullptr;
}

///Return dictionary of all default dialogs in the game.
map<string, std::vector<CDialog*>>& CWorld::getRandomDialogs() {
    return m_defaultDialogs;
}

///Return a random dialog from default dialogs
CDialog* CWorld::getRandomDialog(std::string sID) {
    if(m_defaultDialogs.count(sID) > 0)
    {
        size_t num = rand() % m_defaultDialogs[sID].size();
        return m_defaultDialogs[sID][num];
    }
    std::cout << cRED << "FATAL!!! Accessing dialog which does not exist: " << sID << cCLEAR << std::endl;
    return nullptr;
}


/**
* Return a item. Look for given item in dictionary of items (jsons) and create item from json.
* Return null-pointer if item-json couldn't be found and print error message!
* @param[in] sID id to search item.
* @param[in] p instance of player needed to create an object.
* @return The created item as a pointer.
*/
CItem* CWorld::getItem(string sID, CPlayer* p) { 
    if(m_items.count(sID) > 0)
        return new CItem(m_items[sID], p); 
    else {
        std::cout << "Fatal Error!! Item not found!! (CWord::getItem()): \"" << sID << "\"" << std::endl;
        return nullptr; 
    }
}


void CWorld::worldFactory(CPlayer* p) 
{
    srand(time(NULL));

    //Initialize functions
    CDState::initializeFunctions();
    CItem::initializeFunctions();
    CEnhancedContext::initializeHanlders();
    CEnhancedContext::initializeTemplates();

    //Create attacks
    attackFactory();

    //Create quests
    questFactory();

    //Create items
    itemFactory();

    //Create details
    detailFactory();

    //Create dialogs
    dialogFactory(p);

    //Create default descriptions and dialogs
    defaultDescriptionFactory(p);
    defaultDialogFactory(p);

    //Create characters
    characterFactory();

    //Create rooms
    roomFactory(p);
}

void CWorld::roomFactory(CPlayer* player)
{
    for(auto& p : fs::directory_iterator(m_path_to_world + "/jsons/rooms"))
        roomFactory(p.path(), player);
}

void CWorld::roomFactory(string sPath, CPlayer* p)
{
    //Read json creating all rooms
    std::ifstream read(sPath);
    nlohmann::json j_rooms;
    read >> j_rooms;
    read.close();

    std::string sArea = sPath.substr(sPath.rfind("/")+1, sPath.find(".")-(sPath.rfind("/")+1));
    
    for(auto j_room : j_rooms )
    {
        //Parse characters
        objectmap mapChars = parseRoomChars(j_room, sArea, p);

        //Parse items
        map<string, CItem*> mapItems = parseRoomItems(j_room, sArea, p);

        //Parse details
        map<string, CDetail*> mapDetails = parseRoomDetails(j_room, p, sArea);
        
        //Create new room
        m_rooms[j_room["id"]] = new CRoom(sArea, j_room, mapChars, mapItems, mapDetails, p);
    }
}

void CWorld::detailFactory()
{
    for(auto& p : fs::directory_iterator(m_path_to_world + "jsons/details"))
        detailFactory(p.path());
}

void CWorld::detailFactory(std::string sPath)
{
    //Read json creating all details
    std::ifstream read(sPath);
    nlohmann::json j_details;
    read >> j_details;
    read.close();

    for(auto j_detail: j_details) 
        m_details[j_detail["id"]] = j_detail;
}

map<string, CDetail*> CWorld::parseRoomDetails(nlohmann::json j_room, CPlayer* p, std::string sArea)
{
    map<string, CDetail*> mapDetails;
    if(j_room.count("details") == 0)
        return mapDetails;

    for(auto it : j_room["details"].get<std::vector<nlohmann::json>>())
    {
        //Convert json array to pair and create id
        auto detail = it.get<std::pair<std::string, nlohmann::json>>();
        std::string sID = sArea + "_" + j_room["id"].get<std::string>() + "_" + detail.first; 

        //Get basic json for construction
        nlohmann::json jBasic;
        if(m_details.count(detail.first) > 0)
            jBasic = m_details[detail.first];
        else
            jBasic = {};

        //Update basic with specific json
        func::updateJson(jBasic, detail.second);

        //Update id
        auto lambda = [](CDetail* detail) { return detail->getName(); };
        jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapDetails, lambda), sID);


        //Create items
        parseRandomItemsToDetail(jBasic);
        map<string, CItem*> mapItems = parseRoomItems(jBasic, jBasic["id"], p);

        //Create detail
        mapDetails[jBasic["id"]] = new CDetail(jBasic, p, mapItems);

        //Add [amount] details with "id = id + num" if amount is set.
        for(size_t i=2; i<=detail.second.value("amount", 0u); i++) {
            jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapDetails, lambda), sID);
            mapDetails[jBasic["id"]] = new CDetail(jBasic, p, mapItems);
        }
    }

    return mapDetails;
}

void CWorld::parseRandomItemsToDetail(nlohmann::json& j_detail)
{
    if(j_detail.count("defaultItems") > 0)
    {
        nlohmann::json j = j_detail["defaultItems"];
        int value = j["value"];
        while(value > 0)
        {
            auto it = m_items.begin();
            size_t num = rand() % m_items.size();
            std::advance(it, num);
            nlohmann::json jItem = it->second;
            if(j.count("categories") > 0 && func::inArray(j["categories"], jItem["category"]) == false)
                continue;
            if(j.count("types") > 0 && func::inArray(j["types"], jItem["type"]) == false)
                continue;
            value -= jItem.value("value", 2); 
            nlohmann::json newItem = {it->first, nlohmann::json::object()};
            j_detail["items"].push_back(newItem);
        }
    }
}

void CWorld::itemFactory()
{
    for(auto& p : fs::directory_iterator(m_path_to_world + "jsons/items"))
        itemFactory(p.path());
}

void CWorld::itemFactory(std::string sPath) {
    //Read json creating all items
    std::ifstream read(sPath);
    nlohmann::json j_items;
    read >> j_items;
    read.close();

    for(auto j_item: j_items) 
        m_items[j_item["id"]] = j_item;
}


map<string, CItem*> CWorld::parseRoomItems(nlohmann::json j_room, std::string sArea, CPlayer* p)
{
    map<string, CItem*> mapItems;
    if(j_room.count("items") == 0)
        return mapItems;

    for(auto it : j_room["items"].get<std::vector<nlohmann::json>>()) 
    {
        //Convert json array to pair and create id
        auto item = it.get<std::pair<std::string, nlohmann::json>>();
        std::string sID = sArea + "_" + j_room["id"].get<std::string>() + "_" + item.first; 

        //Get basic json for construction
        nlohmann::json jBasic;
        if(m_items.count(item.first) > 0)
            jBasic = m_items[item.first];
        else
            jBasic = {};

        //Update basic with specific json
        func::updateJson(jBasic, item.second);

        //Update id
        auto lambda = [](CItem* item) { return item->getName(); };
        jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapItems, lambda), sID);

        //Create item
        mapItems[jBasic["id"]] = new CItem(jBasic, p);

        //Add [amount] items with "id = id + num" if amount is set.
        for(size_t i=2; i<=item.second.value("amount", 0u); i++) {
            jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapItems, lambda), sID);
            mapItems[jBasic["id"]] = new CItem(jBasic, p);
        }
    }

    return mapItems;
} 

void CWorld::characterFactory()
{
    for(auto& p : fs::directory_iterator(m_path_to_world + "jsons/characters"))
        characterFactory(p.path());
}

void CWorld::characterFactory(std::string sPath) {
    //Read json creating all characters
    std::ifstream read(sPath);
    nlohmann::json j_chars;
    read >> j_chars;
    read.close();

    for(auto j_char : j_chars) 
        m_jCharacters[j_char["id"]] = j_char;
}

CWorld::objectmap CWorld::parseRoomChars(nlohmann::json j_room, std::string sArea, CPlayer* p)
{
    std::map<std::string, std::string> mapCharacters;
    if(j_room.count("characters") == 0)
        return mapCharacters;

    for(auto it : j_room["characters"].get<vector<nlohmann::json>>())
    {
        //Convert json array to pair and create id
        auto character = it.get<std::pair<std::string, nlohmann::json>>();
        std::string sID = sArea + "_" + j_room["id"].get<std::string>() + "_" + character.first;

        //Gett basic json for construction.
        nlohmann::json jBasic;
        if(m_jCharacters.count(character.first) > 0)
            jBasic = m_jCharacters[character.first];
        else
            jBasic = {};

        //Update basic with specific json
        func::updateJson(jBasic, character.second);     
    
        //Update id
        jBasic["id"] = func::incIDNumber(mapCharacters, sID);

        //Create dialog 
        CDialog* newDialog = new CDialog;
        if(jBasic.count("dialog") > 0)
            newDialog = getDialog(jBasic["dialog"]); 
        else if(jBasic.count("defaultDialog") > 0)
            newDialog = getRandomDialog(jBasic["defaultDialog"]);
        else
            newDialog = getDialog("defaultDialog");

        //Create items and attacks
        map<std::string, CItem*> items = parseRoomItems(jBasic, jBasic["id"], p);
        map<string, CAttack*> attacks = parsePersonAttacks(jBasic);
        
        //Create text
        CText* text = nullptr;
        if(jBasic.count("defaultDescription") > 0)
            text = getRandomDescription(jBasic["defaultDescription"]);

        //Create character and add to maps
        m_characters[jBasic["id"]] = new CPerson(jBasic, newDialog, attacks,text,p,items);
        mapCharacters[jBasic["id"]] = jBasic["name"];

        //Add [amount] characters with "id = id + num" if amount is set.
        for(size_t i=2; i<=character.second.value("amount", 0u); i++) {
            jBasic["id"]  =  func::incIDNumber(mapCharacters, sID);
            m_characters[jBasic["id"]] = new CPerson(jBasic, newDialog, attacks, text, p, items);
            mapCharacters[jBasic["id"]] = jBasic["name"];
        }
    }
    return mapCharacters;
}

void CWorld::attackFactory()
{
    for(auto& p : fs::directory_iterator(m_path_to_world + "jsons/attacks"))
        attackFactory(p.path());
}

void CWorld::attackFactory(std::string sPath) {
    //Read json creating all rooms
    std::ifstream read(sPath);
    nlohmann::json j_attacks;
    read >> j_attacks;
    read.close();

    for(auto j_attack : j_attacks) 
        m_attacks[j_attack["id"]] = new CAttack(j_attack["name"], j_attack["description"], j_attack["output"], j_attack["power"]);
}

map<string, CAttack*> CWorld::parsePersonAttacks(nlohmann::json j_person)
{
    map<string, CAttack*> mapAttacks;
    if(j_person.count("attacks") == 0)
        return mapAttacks;

    objectmap person_attacks = j_person["attacks"].get<objectmap>();
    for(auto attack : person_attacks) 
        mapAttacks[attack.first] = m_attacks[attack.first];

    return mapAttacks;
}


void CWorld::dialogFactory(CPlayer* player)
{
    for(auto& p : fs::directory_iterator(m_path_to_world + "jsons/dialogs"))
    {
        //Read json creating all rooms
        std::ifstream read(p.path());
        nlohmann::json j_states;
        read >> j_states;
        read.close();
        m_dialogs[p.path().stem()] = dialogFactory(j_states, p.path().stem(), player);
    }
}

CDialog* CWorld::dialogFactory(nlohmann::json j_states, std::string sFilename, CPlayer* p)
{
    //Create new dialog
    map<string, CDState*> mapStates;
    CDialog* newDialog = new CDialog("", mapStates);

    for(auto j_state : j_states)
    {
        // *** parse options *** //
        std::map<int, SDOption> options;
        if(j_state.count("options") != 0)
        {
            for(auto& jAtts : j_state["options"])
            {
                nlohmann::json jDeps;
                if(jAtts.count("deps") > 0)
                    jDeps = jAtts["deps"];
            
                options[jAtts["id"]] = {jAtts["text"], jDeps, jAtts["to"]};
            }
        }

        // *** parse state *** //

        //Create state
        mapStates[j_state["id"]] = new CDState(j_state, options, newDialog, p);
    }

    //Update dialog values and return
    newDialog->setName(sFilename);
    newDialog->setStates(mapStates);
    
    //Add dialog to map of all dialogs:
    return newDialog;
}

void CWorld::defaultDescriptionFactory(CPlayer* player)
{
    for(auto& p : fs::directory_iterator(m_path_to_world + "jsons/defaultDescriptions"))
        defaultDescriptionFactory(p.path().stem(), player);
}

void CWorld::defaultDescriptionFactory(std::string sFilename, CPlayer* player)
{
    //Read json creating all default descriptions
    std::ifstream read(m_path_to_world + "jsons/defaultDescriptions/"+sFilename+".json");
    nlohmann::json j_descriptions;
    read >> j_descriptions;
    read.close();

    for(auto j_description : j_descriptions)
        m_defaultDescriptions[sFilename].push_back(new CText(j_description, player));
}

void CWorld::defaultDialogFactory(CPlayer* player)
{
    for(auto& p : fs::directory_iterator(m_path_to_world + "jsons/defaultDialogs"))
    {
        //Read json creating all default descriptions
        std::ifstream read(p.path());
        nlohmann::json j_descriptions;
        read >> j_descriptions;
        read.close();

        for(auto j_dialog : j_descriptions)
            m_defaultDialogs[p.path().stem()].push_back(dialogFactory(j_dialog, p.path().stem(), player));
    }
}

void CWorld::questFactory()
{
    for(auto& p : fs::directory_iterator(m_path_to_world + "jsons/quests"))
        questFactory(p.path());
}

void CWorld::questFactory(std::string sPath)
{
    //Read json creating all quests
    std::ifstream read(sPath);
    nlohmann::json j_quests;
    read >> j_quests;
    read.close();

    for(auto j_quest : j_quests)
    {
        //Create new Quest
        std::map<std::string, CQuestStep*> mapSteps;
        std::map<std::string, std::string> mapHandler;
        CQuest* newQuest = new CQuest(j_quest);
    
        //Create steps
        for(auto j_step : j_quest["steps"]) {
            mapSteps[j_step["id"]] = new CQuestStep(j_step, newQuest);
            if(j_step.count("handler") > 0)
                mapHandler[j_step["id"]] = j_step["handler"];
        }

        //Update quest info
        newQuest->setSteps(mapSteps);
        newQuest->setHandler(mapHandler);
        m_quests[j_quest["id"]] = newQuest;
    }
}


