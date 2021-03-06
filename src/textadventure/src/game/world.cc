#include "world.h"
#include <stdexcept>

#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

namespace fs = std::filesystem;

CWorld::CWorld(std::string path) {
  m_path_to_world = path;
  std::ifstream readConfig(m_path_to_world + "config/config.json");
  readConfig >> m_config;
  readConfig.close();
}

CWorld::CWorld(CPlayer* p, std::string path) {
  m_path_to_world = path;
  worldFactory(p);

  std::ifstream readConfig(m_path_to_world + "config/config.json");
  readConfig >> m_config;
  readConfig.close();
}

// *** GETTER *** // 

std::string CWorld::getPathToWorld() {
  return m_path_to_world;
}

nlohmann::json& CWorld::getConfig() {
  return m_config;
}

map<string, CRoom*>& CWorld::getRooms() { 
  return m_rooms; 
}

CRoom* CWorld::getRoom(std::string sID) {
  if(m_rooms.count(sID) > 0)
    return m_rooms[sID];
  std::cout << cRED << "FATAL! Accessing room which does not exist: " << sID 
    << cCLEAR << std::endl;
  return nullptr;
}
     
map<string, CPerson*>& CWorld::getCharacters() { 
    return m_characters; 
}

CPerson* CWorld::getCharacter(std::string sID) {
  if(m_characters.count(sID) > 0)
      return m_characters[sID];
  std::cout<<cRED<< "FATAL! Accessing character which does not exist: " << 
    sID <<cCLEAR<< std::endl;
  return nullptr;
}

map<string, nlohmann::json>& CWorld::getTexts() {
  return m_jTexts;
}

map<string, CAttack*>& CWorld::getAttacks() { 
  return m_attacks; 
}

CAttack* CWorld::getAttack(std::string sID) {
  if(m_attacks.count(sID) > 0)
    return m_attacks[sID];
  std::cout << cRED << "FATAL! Accessing attack which does not exist: " << 
    sID <<cCLEAR<<std::endl;
  return nullptr;
}

map<string, CQuest*>& CWorld::getQuests() { 
  return m_quests; 
}

CQuest* CWorld::getQuest(std::string sID) {
  if(m_quests.count(sID) > 0)
      return m_quests[sID];
  std::cout << cRED << "FATAL!!! Accessing quest which does not exist: " << 
    sID << cCLEAR << std::endl;
  return nullptr;
}

map<string, CDialog*>& CWorld::getDialogs() {
  return m_dialogs;
}

CDialog* CWorld::getDialog(std::string sID) {
  if(m_dialogs.count(sID) > 0)
    return m_dialogs[sID];
  std::cout << cRED << "FATAL!!! Accessing dialog which does not exist: " << sID << cCLEAR << std::endl;
  return nullptr;
}

map<string, std::vector<CText*>>& CWorld::getRandomDescriptions() {
  return m_defaultDescriptions;
}

CText* CWorld::getRandomDescription(std::string sID) {
  if(m_defaultDescriptions.count(sID) > 0) {
    size_t num = rand() % m_defaultDescriptions[sID].size();
    return m_defaultDescriptions[sID][num];
  }
  std::cout << cRED << "FATAL!!! Accessing description which does not exist: " 
    << sID << cCLEAR << std::endl;
  return nullptr;
}

map<string, std::vector<CDialog*>>& CWorld::getRandomDialogs() {
    return m_defaultDialogs;
}

CDialog* CWorld::getRandomDialog(std::string sID) {
  if(m_defaultDialogs.count(sID) > 0) {
      size_t num = rand() % m_defaultDialogs[sID].size();
      return m_defaultDialogs[sID][num];
  }
  std::cout << cRED << "FATAL!!! Accessing dialog which does not exist: " << sID << cCLEAR << std::endl;
  return nullptr;
}

CItem* CWorld::getItem(string sID, CPlayer* p) { 
  if(m_items.count(sID) > 0)
    return new CItem(m_items[sID], p); 
  else {
    std::cout << "Fatal Error!! Item not found!! (CWord::getItem()): \"" 
      << sID << "\"" << std::endl;
    return nullptr; 
  }
}


void CWorld::worldFactory(CPlayer* p) {
  srand(time(NULL));

  //Initialize functions
  CDState::initializeFunctions();
  CItem::initializeFunctions();
  Context::initializeHanlders();
  Context::initializeTemplates();

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

  // Verify existing exists 
  for (auto it : m_rooms) {
    for (auto exit : it.second->getExtits()) {
      if (m_rooms.count(exit.first) == 0) {
        std::cout << cRED "Exit without matching room: " + exit.first + " in " 
          + it.first << cCLEAR << std::endl;
        throw cRED "Exit without matching room: " + exit.first + " in " + it.first + cCLEAR;
      }
    }
  }

  //Load texts
  textFactory();
  std::cout << "worldFactory: done." << std::endl;
}

void CWorld::roomFactory(CPlayer* player) {
  for(auto& p : fs::directory_iterator(m_path_to_world + "rooms"))
    roomFactory(p.path(), player);
}

void CWorld::roomFactory(string sPath, CPlayer* p) {
  //Read json creating all rooms
  std::ifstream read(sPath);
  nlohmann::json j_rooms;
  read >> j_rooms;
  read.close();

  fs::path path = sPath;
  std::string sArea = path.stem();
  
  for(auto j_room : j_rooms ) {
    // Add the area id to the room-id.
    j_room["id"] = sArea + "." + j_room["id"].get<std::string>();
    std::cout << "Room: " << j_room["id"] << std::endl;

    std::map<std::string, CPerson*> mapChars = parseRoomChars(j_room, p);
    map<string, CItem*> mapItems = parseRoomItems(j_room, p);
    map<string, CDetail*> mapDetails = parseRoomDetails(j_room, p);
    m_rooms[j_room["id"]] = new CRoom(sArea, j_room, mapChars, mapItems, mapDetails, p);
    std::cout << "Create room: " << j_room["id"] << std::endl;
  }
  std::cout << "Created all rooms " << std::endl;
}

void CWorld::detailFactory() {
  for(auto& p : fs::directory_iterator(m_path_to_world + "details"))
    detailFactory(p.path());
}

void CWorld::detailFactory(std::string sPath) {
  //Read json creating all details
  std::ifstream read(sPath);
  nlohmann::json j_details;
  read >> j_details;
  read.close();

  for(auto j_detail: j_details) 
    m_details[j_detail["id"]] = j_detail;
}

map<string, CDetail*> CWorld::parseRoomDetails(nlohmann::json j_room, 
    CPlayer* p) {
  std::cout << "Parsing details \n";
  map<string, CDetail*> mapDetails;
  if(j_room.count("details") == 0)
    return mapDetails;

  for(auto it : j_room["details"].get<std::vector<nlohmann::json>>()) {
    //Convert json array to pair and create id by adding the room id.
    auto detail = it.get<std::pair<std::string, nlohmann::json>>();
    std::string sID = j_room["id"].get<std::string>() + "." + detail.first; 

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
    jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapDetails,lambda), sID);

    //Create items
    std::cout << jBasic["id"] << std::endl;
    parseRandomItemsToDetail(jBasic);
    map<string, CItem*> mapItems = parseRoomItems(jBasic, p);

    //Create detail
    mapDetails[jBasic["id"]] = new CDetail(jBasic, p, mapItems);

    //Add [amount] details with "id = id + num" if amount is set.
    for(size_t i=2; i<=detail.second.value("amount", 0u); i++) {
      jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapDetails, 
            lambda), sID);
      mapDetails[jBasic["id"]] = new CDetail(jBasic, p, mapItems);
    }
  }

  return mapDetails;
}

void CWorld::parseRandomItemsToDetail(nlohmann::json& j_detail) {
  std::cout << "parseRandomItemsToDetail" << std::endl;
  if(j_detail.count("defaultItems") > 0) {
    nlohmann::json j = j_detail["defaultItems"];
    int value = j["value"];
    int counter = 0;
    while(value > 0) {
      counter++;
      if (counter == 10000) {
        std::cout << "Wrong values set for type/ categories: no items found." << std::endl;
        throw("Wrong entries for defaultItems");
        break;
      }
      auto it = m_items.begin();
      size_t num = rand() % m_items.size();
      std::advance(it, num);
      nlohmann::json jItem = it->second;
      if(j.count("categories") > 0 && j["categories"].size() > 0 
          && func::inArray(j["categories"], jItem["category"]) == false) continue;
      if(j.count("types") > 0 && j["types"].size() > 0 
          && func::inArray(j["types"], jItem["type"]) == false) continue;
      value -= jItem.value("value", 2); 
      nlohmann::json newItem = {it->first, nlohmann::json::object()};
      j_detail["items"].push_back(newItem);
    }
  }
}

void CWorld::itemFactory() {
  for (auto& p : fs::directory_iterator(m_path_to_world + "items"))
    itemFactory(p.path());
}

void CWorld::itemFactory(std::string sPath) {
  //Read json creating all items
  std::ifstream read(sPath);
  nlohmann::json j_items;
  read >> j_items;
  read.close();

  for (auto j_item: j_items) 
    m_items[j_item["id"]] = j_item;
}


map<string, CItem*> CWorld::parseRoomItems(nlohmann::json j_room, CPlayer* p) {
  std::cout << "Parsing Items\n";
  map<string, CItem*> mapItems;
  if (j_room.count("items") == 0)
    return mapItems;

  for(auto it : j_room["items"].get<std::vector<nlohmann::json>>()) {
    // Convert json array to pair and create id
    auto item = it.get<std::pair<std::string, nlohmann::json>>();
    std::string sID = j_room["id"].get<std::string>() + "." + item.first; 

    // Get basic json for construction
    nlohmann::json jBasic;
    if(m_items.count(item.first) > 0)
      jBasic = m_items[item.first];
    else
      jBasic = {};

    //Update basic with specific json
    func::updateJson(jBasic, item.second);

    // Update id
    auto lambda = [](CItem* item) { return item->getName(); };
    jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapItems, lambda), sID);

    // Create item
    mapItems[jBasic["id"]] = new CItem(jBasic, p);

    // Add [amount] items with "id = id + num" if amount is set.
    for (size_t i=2; i<=item.second.value("amount", 0u); i++) {
      jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapItems, lambda), sID);
      mapItems[jBasic["id"]] = new CItem(jBasic, p);
    }
  }

  return mapItems;
} 

void CWorld::characterFactory() {
  for (auto& p : fs::directory_iterator(m_path_to_world + "characters"))
    characterFactory(p.path());
}

void CWorld::characterFactory(std::string sPath) {
  // Read json creating all characters
  std::ifstream read(sPath);
  nlohmann::json j_chars;
  read >> j_chars;
  read.close();

  for (auto j_char : j_chars) 
    m_jCharacters[j_char["id"]] = j_char;
}

std::map<std::string, CPerson*> CWorld::parseRoomChars(nlohmann::json j_room, 
    CPlayer* p) {
  std::cout << "Parsing characters\n";
  std::map<std::string, CPerson*> mapCharacters;
  if (j_room.count("characters") == 0)
    return mapCharacters;

  for (auto it : j_room["characters"].get<vector<nlohmann::json>>()) {
    // Convert json array to pair and create id
    auto character = it.get<std::pair<std::string, nlohmann::json>>();
    std::string sID = j_room["id"].get<std::string>() + "." + character.first;

    // Get basic json for construction.
    nlohmann::json jBasic;
    if (m_jCharacters.count(character.first) > 0)
      jBasic = m_jCharacters[character.first];
    else
      jBasic = {};

    // Update basic with specific json
    func::updateJson(jBasic, character.second);     

    if (jBasic.count("name") == 0) {
      std::cout << cRED "Character created without name: " << sID << cCLEAR << std::endl; 
      throw cRED "Character created without name: " + sID + cCLEAR;
    }

    // Update id
    auto lambda = [] (CPerson* person) { return person->getName(); };
    jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapCharacters, lambda), sID);

    // ** Create dialog ** //
    
    // Load all dialogs 
    std::map<std::string, CDialog*> dialogs;
    for (auto it : m_dialogs) {
      if (it.first.find(character.first) != std::string::npos)
        dialogs[it.first.substr(character.first.length()+1)] = it.second;
    }

    // Select main dialog
    CDialog* newDialog = new CDialog;
    if (dialogs.count("1") > 0)                  //Load standard dialog [id]_1
      newDialog = dialogs["1"];
    else if (jBasic.count("dialog") > 0)         //Load special dialog 
      newDialog = getDialog(jBasic["dialog"]); 
    else if (jBasic.count("defaultDialog") > 0)  //Load a random default dialogue
      newDialog = getRandomDialog(jBasic["defaultDialog"]);
    else
      newDialog = getDialog("defaultDialog"); //Load the standard default-dialogue

    // Create items and attacks
    map<std::string, CItem*> items = parseRoomItems(jBasic, p);
    map<string, CAttack*> attacks = parsePersonAttacks(jBasic);
    
    // Create text
    CText* text = nullptr;
    if (jBasic.count("defaultDescription") > 0)
      text = getRandomDescription(jBasic["defaultDescription"]);

    // Create character and add to maps
    m_characters[jBasic["id"]] = new CPerson(jBasic, newDialog, attacks,text, p, dialogs, items);
    mapCharacters[jBasic["id"]] = m_characters[jBasic["id"]];

    //Add [amount] characters with "id = id + num" if amount is set.
    for(size_t i=2; i<=character.second.value("amount", 0u); i++) {
        jBasic["id"]  =  func::incIDNumber(func::convertToObjectmap(mapCharacters, lambda), sID);
        m_characters[jBasic["id"]] = new CPerson(jBasic, newDialog, attacks, text, p, dialogs, items);
        mapCharacters[jBasic["id"]] = m_characters[jBasic["id"]];
    }
  }
  return mapCharacters;
}

void CWorld::attackFactory() {
  for(auto& p : fs::directory_iterator(m_path_to_world + "attacks"))
    attackFactory(p.path());
}

void CWorld::attackFactory(std::string sPath) {
  //Read json creating all rooms
  std::ifstream read(sPath);
  nlohmann::json j_attacks;
  read >> j_attacks;
  read.close();

  for(auto j_attack : j_attacks)
    m_attacks[j_attack["id"]] = new CAttack(j_attack.value("name", ""), 
      j_attack.value("description", ""), j_attack.value("output", ""), 
      j_attack.value("power", 0)
    );
}

map<string, CAttack*> CWorld::parsePersonAttacks(nlohmann::json j_person) {
  map<string, CAttack*> mapAttacks;
  if (j_person.count("attacks") == 0)
    return mapAttacks;

  objectmap person_attacks = j_person["attacks"].get<objectmap>();
  for (auto attack : person_attacks) 
    mapAttacks[attack.first] = m_attacks[attack.first];

  return mapAttacks;
}


void CWorld::dialogFactory(CPlayer* player) {
  for (auto& p : fs::directory_iterator(m_path_to_world + "dialogs")) {
    //Read json creating all rooms
    std::ifstream read(p.path());
    nlohmann::json j_states;
    read >> j_states;
    read.close();
    m_dialogs[p.path().stem()] = dialogFactory(j_states, p.path().stem(), player);
  }
}

CDialog* CWorld::dialogFactory(nlohmann::json j_states, std::string sFilename, 
    CPlayer* p) {
  //Create new dialog
  map<string, CDState*> mapStates;
  CDialog* newDialog = new CDialog("", mapStates);

  for (auto j_state : j_states) {
    // *** parse options *** //
    std::map<int, SDOption> options;
    if (j_state.count("options") != 0) {
      for (auto& jAtts : j_state["options"]) {
        std::string logic = jAtts.value("logic", "");
        options[jAtts["id"]] = {jAtts["text"], logic, jAtts["to"], false};
      }
    }
    //Create state
    mapStates[j_state["id"]] = new CDState(j_state, options, newDialog, p);
  }

  //Update dialog values and return
  newDialog->setName(sFilename);
  newDialog->setStates(mapStates);
  
  //Add dialog to map of all dialogs:
  return newDialog;
}

void CWorld::defaultDescriptionFactory(CPlayer* player) {
  for(auto& p : fs::directory_iterator(m_path_to_world+"defaultDescriptions"))
    defaultDescriptionFactory(p.path().stem(), player);
}

void CWorld::defaultDescriptionFactory(std::string sFilename, CPlayer* player) {
  //Read json creating all default descriptions
  std::ifstream read(m_path_to_world+"defaultDescriptions/"+sFilename+".json");
  nlohmann::json j_descriptions;
  read >> j_descriptions;
  read.close();

  for(auto j_description : j_descriptions)
    m_defaultDescriptions[sFilename].push_back(new CText(j_description,player));
}

void CWorld::defaultDialogFactory(CPlayer* player) {
  for(auto& p : fs::directory_iterator(m_path_to_world + "defaultDialogs")) {
    //Read json creating all default descriptions
    std::ifstream read(p.path());
    nlohmann::json j_descriptions;
    read >> j_descriptions;
    read.close();

    for(auto j_dialog : j_descriptions)
      m_defaultDialogs[p.path().stem()].push_back(dialogFactory(j_dialog, 
            p.path().stem(), player));
  }
}

void CWorld::questFactory() {
  for(auto& p : fs::directory_iterator(m_path_to_world + "quests"))
    questFactory(p.path());
}

void CWorld::questFactory(std::string sPath) {
  std::cout << "QuestFactory"<<std::endl;
  //Read json creating all quests
  std::ifstream read(sPath);
  nlohmann::json j_quests;
  read >> j_quests;
  read.close();

  for (auto j_quest : j_quests) {
    //Create new Quest
    std::map<std::string, CQuestStep*> mapSteps;
    CQuest* newQuest = new CQuest(j_quest);

    //Create steps
    for (auto j_step : j_quest.value("steps", std::vector<nlohmann::json>())) {
      std::cout << j_step["id"] << std::endl;
      mapSteps[j_step["id"]] = new CQuestStep(j_step, newQuest);
    }

    //Get listeners for this quest.
    std::vector<nlohmann::json> listeners;
    if (j_quest.count("listener") > 0 && j_quest["listener"].size() > 0)
      listeners = j_quest["listener"].get<std::vector<nlohmann::json>>();

    //Update quest info
    newQuest->setSteps(mapSteps);
    newQuest->setHandler(listeners);
    m_quests[j_quest["id"]] = newQuest;
  }
}

void CWorld::textFactory() {
  for(auto& p : fs::directory_iterator(m_path_to_world + "texts"))
    textFactory(p.path()); 
}

void CWorld::textFactory(std::string sPath) {
  //Read json creating all quests
  std::ifstream read(sPath);
  nlohmann::json j_texts;
  read >> j_texts;
  read.close();

  for(auto it=j_texts.begin(); it!=j_texts.end(); it++)
    m_jTexts[it.key()] = it.value()["text"];
}
