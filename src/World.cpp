#include "CWorld.hpp"

#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

namespace fs = std::experimental::filesystem;

CWorld::CWorld() {}
CWorld::CWorld(CPlayer* p) {
    worldFactory(p);
}

// *** GETTER *** // 

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
    std::cout << cRED << "FATAL!!! Accessing room which does not exist: " << sID << cCLEAR << std::endl;
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

    //Create characters
    characterFactory();

    //Create rooms
    roomFactory(p);
}

void CWorld::roomFactory(CPlayer* player)
{
    for(auto& p : fs::directory_iterator("factory/jsons/rooms"))
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
        map<string, CDetail*> mapDetails = detailFactory(j_room, p, sArea);
        
        //Create new room
        m_rooms[j_room["id"]] = new CRoom(sArea, j_room, mapChars, mapItems, mapDetails, p);
    }
}

map<string, CDetail*> CWorld::detailFactory(nlohmann::json j_room, CPlayer* p, std::string sArea)
{
    map<string, CDetail*> mapDetails;
    if(j_room.count("details") == 0)
        return mapDetails;

    for(auto j_detail : j_room["details"])
        mapDetails[j_detail["id"]] = new CDetail(j_detail, p, sArea + "_" + j_room["id"].get<std::string>());

    return mapDetails;
}

void CWorld::itemFactory()
{
    for(auto& p : fs::directory_iterator("factory/jsons/items"))
        itemFactory(p.path());
}

void CWorld::itemFactory(std::string sPath) {
    //Read json creating all rooms
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
        //Convert json array to par and create id
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
    for(auto& p : fs::directory_iterator("factory/jsons/characters"))
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
            newDialog = dialogFactory(jBasic["dialog"], p); 
        else
            newDialog = dialogFactory("defaultDialog", p);

        //Create items and attacks
        map<std::string, CItem*> items = parseRoomItems(jBasic, jBasic["id"], p);
        map<string, CAttack*> attacks = parsePersonAttacks(jBasic);

        //Create character and add to maps
        m_characters[jBasic["id"]] = new CPerson(jBasic, newDialog, attacks, p, items);
        mapCharacters[jBasic["id"]] = jBasic["name"];

        //Add [amount] characters with "id = id + num" if amount is set.
        for(size_t i=2; i<=character.second.value("amount", 0u); i++) {
            jBasic["id"]  =  func::incIDNumber(mapCharacters, sID);
            m_characters[jBasic["id"]] = new CPerson(jBasic, newDialog, attacks, p, items);
            mapCharacters[jBasic["id"]] = jBasic["name"];
        }
    }
    return mapCharacters;
}

void CWorld::attackFactory()
{
    for(auto& p : fs::directory_iterator("factory/jsons/attacks"))
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


CDialog* CWorld::dialogFactory(string sPath, CPlayer* p)
{
    //Read json creating all rooms
    std::ifstream read("factory/jsons/dialogs/"+sPath+".json");
    nlohmann::json j_states;
    read >> j_states;
    read.close();

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
    newDialog->setName(sPath);
    newDialog->setStates(mapStates);

    return newDialog;
}

void CWorld::questFactory()
{
    for(auto& p : fs::directory_iterator("factory/jsons/quests"))
        questFactory(p.path());
}

void CWorld::questFactory(std::string sPath)
{
    //Read json creating all rooms
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


