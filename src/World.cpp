#include "CWorld.hpp"

namespace fs = std::experimental::filesystem;

CWorld::CWorld() {
    worldFactory();
}

void CWorld::worldFactory() 
{
    //Initialize functions
    CDState::initializeFunctions();
    CItem::initializeFunctions();

    //Create attacks
    attackFactory();

    //Create items
    itemFactory();

    //Create rooms
    roomFactory();
}

void CWorld::roomFactory()
{
    for(auto& p : fs::directory_iterator("factory/jsons/rooms"))
        roomFactory(p.path());
}


void CWorld::roomFactory(string sPath)
{
    //Read json creating all rooms
    std::ifstream read(sPath);
    nlohmann::json j_rooms;
    read >> j_rooms;
    read.close();
    
    for(auto j_room : j_rooms )
    {
        //Parse characters 
        objectmap mapChars = characterFactory(j_room["characters"]);

        //Parse items
        map<string, CItem*> mapItems = parseRoomItems(j_room);

        //Parse details
        map<string, CDetail*> mapDetails = detailFactory(j_room);

        //Create new room
        m_rooms[j_room["id"]] = new CRoom(j_room["name"], j_room["id"], j_room["description"], j_room.value("entry", ""), j_room["exits"], mapChars, mapItems, mapDetails);
    }
}

map<string, CDetail*> CWorld::detailFactory(nlohmann::json j_room)
{
    map<string, CDetail*> mapDetails;
    if(j_room.count("details") == 0)
        return mapDetails;

    for(auto j_detail : j_room["details"])
    {
        objectmap characters;
        if(j_detail.count("characters") > 0) 
            characters = j_detail["characters"].get<objectmap>();
        objectmap items;
        if(j_detail.count("items") > 0) 
            items = j_detail["items"].get<objectmap>();

        mapDetails[j_detail["id"]] = new CDetail(j_detail["name"], j_detail["id"], j_detail["description"], j_detail["look"], characters, items);
    }

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


map<string, CItem*> CWorld::parseRoomItems(nlohmann::json j_room)
{
    map<string, CItem*> mapItems;
    if(j_room.count("items") == 0)
        return mapItems;

    for(auto j_item : j_room["items"])
        mapItems[j_item["id"]] = new CItem(m_items[j_item["from"]], j_item);

    return mapItems;
} 

CWorld::objectmap CWorld::characterFactory(nlohmann::json j_characters)
{
    objectmap mapChars;
    for(auto j_char : j_characters)
    {

        //Create dialog 
        SDialog* newDialog = new SDialog;
        if(j_char.count("dialog") > 0)
            newDialog = dialogFactory(j_char["dialog"]); 
        else
            newDialog = dialogFactory("defaultDialog");

        //Create attacks
        map<string, CAttack*> attacks = parsePersonAttacks(j_char);

        //Create character and add to maps
        m_characters[j_char["id"]] = new CCharacter(j_char["name"], j_char["id"], j_char.value("description",""), j_char.value("hp", 30), j_char.value("strength", 7), newDialog, attacks);
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


SDialog* CWorld::dialogFactory(string sPath)
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
            for(auto it : mapOptions)
            {
                std::vector<string> vAtts = func::split(it.second, ";");
                nlohmann::json jDeps = nlohmann::json::parse(vAtts[1]);
                options[stoi(it.first)] = {vAtts[0], jDeps, vAtts[2]};
            }
        }

        // *** parse state *** //

        //Parse alternative texts
        vector<string> altTexts;
        if(j_state.count("altTexts") > 0) 
            altTexts = j_state["altTexts"].get<vector<string>>();

        //Create state
        mapStates[j_state["id"]] = new CDState(j_state["text"], j_state.value("function", "standard"), altTexts, options, newDialog);
    }

    //Update dialog values and return
    newDialog->sName = sPath;
    newDialog->states= mapStates;
    return newDialog;
}
