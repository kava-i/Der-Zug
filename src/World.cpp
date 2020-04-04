#include "CWorld.hpp"

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

///Return dictionary of all characters aka people in the game.
map<string, CPerson*>& CWorld::getCharacters() { 
    return m_characters; 
}

///Return dictionary of all attacks in the game.
map<string, CAttack*>& CWorld::getAttacks() { 
    return m_attacks; 
}

///Return dictionary of all quests in the game.
map<string, CQuest*>& CWorld::getQuests() { 
    return m_quests; 
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
        objectmap mapChars;
        if(j_room.count("characters") > 0)
            mapChars = characterFactory(j_room["characters"], p);

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
        mapDetails[j_detail["id"]] = new CDetail(j_detail, p, sArea);

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

    if(m_items.count("ticket") > 0)
        std::cout << "Created TICKET.\n";
}


map<string, CItem*> CWorld::parseRoomItems(nlohmann::json j_room, std::string sArea, CPlayer* p)
{
    map<string, CItem*> mapItems;
    if(j_room.count("items") == 0)
        return mapItems;

    map<string, nlohmann::json> items = j_room["items"].get<map<string, nlohmann::json>>();
    for(auto it : items) 
    {
        std::string sID = sArea + "_" + it.first; 

        nlohmann::json jBasic;
        if(m_items.count(it.first) > 0)
            jBasic = m_items[it.first];
        else
            jBasic = {};

        mapItems[sID] = new CItem(jBasic, it.second, p, sID);

        //Add [amount] items with "id = id + num" if amount is set.
        for(size_t i=2; i<=it.second.value("amount", 0u); i++) {
            std::string newID =  sID +std::to_string(i);
            mapItems[newID] = new CItem(jBasic, it.second, p, newID);
        }
    }

    return mapItems;
} 

CWorld::objectmap CWorld::characterFactory(nlohmann::json j_characters, CPlayer* p)
{
    objectmap mapChars;
    for(auto j_char : j_characters)
    {
        //Create dialog 
        SDialog* newDialog = new SDialog;
        if(j_char.count("dialog") > 0)
            newDialog = dialogFactory(j_char["dialog"], p); 
        else
            newDialog = dialogFactory("defaultDialog", p);

        //Create items and attacks
        map<std::string, CItem*> items = parseRoomItems(j_char, j_char["id"], p);
        map<string, CAttack*> attacks = parsePersonAttacks(j_char);

        //Create character and add to maps
        m_characters[j_char["id"]] = new CPerson(j_char, newDialog, attacks, p, items);
        mapChars[j_char["id"]] = j_char["name"];
    }

    return mapChars;
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


SDialog* CWorld::dialogFactory(string sPath, CPlayer* p)
{
    //Read json creating all rooms
    std::ifstream read("factory/jsons/dialogs/"+sPath+".json");
    nlohmann::json j_states;
    read >> j_states;
    read.close();

    //Create new dialog
    map<string, CDState*> mapStates;
    struct SDialog* newDialog = new SDialog();

    for(auto j_state : j_states)
    {
        // *** parse options *** //
        map<int, SDOption> options;
        if(j_state.count("options") != 0)
        {
            std::map<string, string> mapOptions = j_state["options"].get<std::map<string, string>>(); 
            for(auto it : mapOptions) {
                std::vector<string> vAtts = func::split(it.second, ";");
                nlohmann::json jDeps = nlohmann::json::parse(vAtts[1]);
                options[stoi(it.first)] = {vAtts[0], jDeps, vAtts[2]};
            }
        }

        // *** parse state *** //

        //Create state
        mapStates[j_state["id"]] = new CDState(j_state, options, newDialog, p);
    }

    //Update dialog values and return
    newDialog->sName = sPath;
    newDialog->states= mapStates;
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


