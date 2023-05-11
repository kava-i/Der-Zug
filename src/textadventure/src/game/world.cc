#include "world.h"
#include "eventmanager/listener.h"
#include "game/config/attributes.h"
#include "nlohmann/json_fwd.hpp"
#include "objects/detail.h"
#include "tools/func.h"
#include <cstddef>
#include <exception>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <objects/player.h>
#include <vector>
#include "tools/exceptions.h"

#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

namespace fs = std::filesystem;

CWorld::CWorld(CPlayer* p, std::string path) {
  m_path_to_world = path;
  // Load config
  configFactory("config/config.json");

	try {
		attribute_config_ = AttributeConfig(func::LoadJsonFromDisc("config/attributes.json"));
	} catch(std::exception& e) {
		std::string msg = "Failed parsing attribute-config: ";
		msg + e.what();
    throw WorldFactoryException(msg);
	}
  worldFactory(p);
  // Extract f.e. media files from config
  configExtractor(p);
}

// *** GETTER *** // 

std::string CWorld::getPathToWorld() {
  return m_path_to_world;
}

nlohmann::json& CWorld::getConfig() {
  return m_config;
}

const AttributeConfig& CWorld::attribute_config() {
	return attribute_config_;
}

std::string CWorld::GetSTDText(std::string key) {
  // If text not given in config, print `key` only.
  if (!m_config["translations"].contains(key)) {
    std::cout << cRED << "missing translation for: " << key << cCLEAR << std::endl;
    return key;
  }
  return m_config["translations"][key];
}

std::string CWorld::media(std::string type) const {
  return (media_.count(type) > 0) ? media_.at(type) : "";
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

map<string, nlohmann::json> CWorld::getTexts(std::string area) {
  std::map<std::string, nlohmann::json> texts;
  for (const auto& it : m_jTexts) {
    // if area is specified only get texts in specifc area.
    if (area == "" || it.second.first == area)
      texts[it.first] = it.second.second;
  }
  return texts;
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

CText* CWorld::getRandomDescription(std::string id, CPlayer* p) {
  auto texts_in_area = getTexts(id);

  if (texts_in_area.size() == 0) {
    std::cout << cRED << "FATAL!!! Accessing description which does not exist: " 
      << id << cCLEAR << std::endl;
    return nullptr;
  }
  size_t num = rand() % texts_in_area.size();
  auto item = texts_in_area.begin();
  std::advance(item, num);
  return new CText(item->second, p);
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

  //Load texts
  textFactory();

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

  //Create characters
  characterFactory();

  //Create rooms
  roomFactory(p);

  // Verify existing exists 
  for (auto it : m_rooms) {
    for (auto jt : it.second->getExtits()) {
      if (m_rooms.count(it.first) == 0)
        throw WorldFactoryException("Exits without matching room: " + it.first + " in " + it.first);
    }
  }

  std::cout << "worldFactory: done." << std::endl;
}

void CWorld::configFactory(const std::string path) {
  std::cout << "Loading config. " << std::endl;
  m_config = func::LoadJsonFromDisc(m_path_to_world + path);
  if (m_config.size() == 0)
    throw WorldFactoryException("Missing config at " + path);

  // Check that all show-attributes are set.
  if (!m_config.contains("show"))
    throw WorldFactoryException("Missing 'show' entry in config!");
  else {
    for (const std::string it : {"chars", "exits", "items", "details", "add_all_items"}) {
      if (!m_config["show"].contains(it) || m_config["show"][it].size() < 2)
        throw WorldFactoryException("Missing '" + it + "' entry in 'show' entry of config!");
    }
  }
}

void CWorld::configExtractor(CPlayer *p) {
  if (m_config.contains("music")) {
    for (const auto& [key, value] : m_config["music"].items()) {
      // For background, overwite general setting with player specifc setting.
      if (key == "background" && p && p->music() != "") {
        media_["music/"+key] = value;
        p->set_music(""); // afterwards empty music.
      }
      else
        media_["music/"+key] = value;
    }
  }
  if (m_config.contains("image")) {
    for (const auto& [key, value] : m_config["image"].items()) {
      if (key == "background" && p && p->music() != "") {
        media_["image/"+key] = value;
        p->set_image(""); // afterwards empty music.
      }
      else 
        media_["image/"+key] = value;
    }
  }
  std::cout << "Media set. " << std::endl;
}

void CWorld::roomFactory(CPlayer* player) {
  for(auto& p : fs::directory_iterator(m_path_to_world + "rooms")) {
    roomFactory(p.path(), player);
  }
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
    std::cout << "Done creating details" << std::endl;
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

map<string, CDetail*> CWorld::parseRoomDetails(nlohmann::json j_room, CPlayer* p) {
  std::cout << "Parsing details \n";
  map<string, CDetail*> map_details;
  if (j_room.count("details") == 0)
    return map_details;

  for (auto it : j_room["details"].get<std::vector<nlohmann::json>>()) {
    //Convert json array to pair and create id by adding the room id.
    auto detail = it.get<std::pair<std::string, nlohmann::json>>();
    std::string id = j_room["id"].get<std::string>() + "." + detail.first; 

    //Get basic json for construction
    nlohmann::json template_detail_json;
    if(m_details.count(detail.first) > 0)
      template_detail_json = m_details[detail.first];
    else
      template_detail_json = {};

    //Update basic with specific json
    func::updateJson(template_detail_json, detail.second);

    //Update id
    auto lambda = [](CDetail* detail) { return detail->name(); };
    template_detail_json["id"] = func::incIDNumber(func::convertToObjectmap(map_details,lambda), id);

    //Create items
    std::cout << template_detail_json["id"] << std::endl;
    parseRandomItemsToDetail(template_detail_json);
    map<string, CItem*> mapItems = parseRoomItems(template_detail_json, p);

    //Create detail
    map_details[template_detail_json["id"]] = new CDetail(template_detail_json, p, mapItems);

    //Add [amount] details with "id = id + num" if amount is set.
    for (size_t i=2; i<=detail.second.value("amount", 0u); i++) {
      template_detail_json["id"] = func::incIDNumber(func::convertToObjectmap(map_details, lambda), id);
      map_details[template_detail_json["id"]] = new CDetail(template_detail_json, p, mapItems);
    }
  }
  return map_details;
}

CDetail* CWorld::GetDetail(std::string id, std::string room_id, CPlayer* p) {
  // Check that room and detail exists.
  auto room = getRoom(room_id);
  if (m_details.count(id) > 0 && room != nullptr) {
    nlohmann::json template_detail_json = m_details.at(id);
    //Update id
    auto lambda = [](CDetail* detail) { return detail->name(); };
    template_detail_json["id"] = func::incIDNumber(func::convertToObjectmap(room->getDetails(), lambda), id);
    parseRandomItemsToDetail(template_detail_json);
    map<string, CItem*> mapItems = parseRoomItems(template_detail_json, p);
    return new CDetail(template_detail_json, p, mapItems);
  }
  return nullptr;
}

void CWorld::parseRandomItemsToDetail(nlohmann::json& j_detail) {
  std::cout << "parseRandomItemsToDetail" << std::endl;
  // Only add random items if defaultItems is set.
  if (j_detail.count("defaultItems") > 0) {
    nlohmann::json j = j_detail["defaultItems"];
    int value = j["value"];
    int counter = 0;
    while(value > 0) {
      counter++;
      if (counter == 10000) {
        throw WorldFactoryException("Wrong values set for type/ categories: no items found.");
      }
      auto it = m_items.begin();
      size_t num = rand() % m_items.size();
      std::advance(it, num);
      nlohmann::json jItem = it->second;
      if(j.count("categories") > 0 && j["categories"].size() > 0 
          && func::inArray(j["categories"], jItem["category"]) == false) 
        continue;
      if(j.count("types") > 0 && j["types"].size() > 0 
          && func::inArray(j["types"], jItem["type"]) == false) 
        continue;
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
  std::cout << "Parsing Items" << std::endl;
  map<string, CItem*> mapItems;
  if (j_room.count("items") == 0)
    return mapItems;

  for(auto it : j_room.value("items", std::vector<nlohmann::json>())) {
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
    auto lambda = [](CItem* item) { return item->name(); };
    jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapItems, lambda), sID);

    // Create item
    mapItems[jBasic["id"]] = new CItem(jBasic, p);

    // Add [amount] items with "id = id + num" if amount is set.
    for (size_t i=2; i<=item.second.value("amount", 0u); i++) {
      jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapItems, lambda), sID);
      mapItems[jBasic["id"]] = new CItem(jBasic, p);
    }
  }
  std::cout << "done" << std::endl;
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
      throw WorldFactoryException("Character created without name: " + sID);
    }

    // Update id
    auto lambda = [] (CPerson* person) { return person->name(); };
    jBasic["id"] = func::incIDNumber(func::convertToObjectmap(mapCharacters, lambda), sID);

    // ** Create dialog ** //
    
    // Load all dialogs of either character, of if defaultDialog is specified of
    // this defaultDialog-type
    std::map<std::string, CDialog*> dialogs;
    std::string dialog_name = jBasic.value("defaultDialog", character.first);
    size_t counter = 0;
    for (auto it : m_dialogs) {
      if (it.first.find(dialog_name) != std::string::npos) {
        dialogs[it.first.substr(dialog_name.length()+1)] = it.second;
        counter++;
      }
    }
    // Pick either a random (default-dialog), or the first (character-dialog).
    std::cout << "Loading character dialog" << std::endl;
    std::string dialog_num = (jBasic.contains("defaultDialog")) ? std::to_string(rand() % counter) : "1"; 
    std::cout << "Got dialog num: " << std::endl;
    CDialog* newDialog = (dialogs.count(dialog_num) > 0) ? dialogs[dialog_num] : getDialog("defaultDialog");
    std::cout << "Loaded dialog: " << std::endl;

    // Create items and attacks
    map<std::string, CItem*> items = parseRoomItems(jBasic, p);
    map<string, CAttack*> attacks = parsePersonAttacks(jBasic.value("attacks", std::vector<std::string>()));
    
    // Create text
    CText* text = nullptr;
    if (jBasic.count("defaultDescription") > 0)
      text = getRandomDescription(jBasic["defaultDescription"], p);

    // Create character and add to maps
    m_characters[jBasic["id"]] = new CPerson(jBasic, newDialog, attacks, text, p, dialogs, items);
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

map<string, CAttack*> CWorld::parsePersonAttacks(std::vector<std::string> attack_ids) {
  map<string, CAttack*> mapAttacks;
  for (const auto& attack_id : attack_ids) 
    mapAttacks[attack_id] = m_attacks[attack_id];
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
    std::vector<CListener*> listeners;
    for (const auto& it : j_quest.value("listeners", std::vector<nlohmann::json>())) {
			auto new_listener = CListener::FromJson(it, "quest", newQuest->getID());
			listeners.push_back(new_listener);
    }
    //Update quest info
    newQuest->setSteps(mapSteps);
    newQuest->setListeners(listeners);
    m_quests[j_quest["id"]] = newQuest;
  }
}

void CWorld::textFactory() {
  for(auto& p : fs::directory_iterator(m_path_to_world + "texts"))
    textFactory(p.path()); 
}

void CWorld::textFactory(std::string path) {
  //Read json creating all quests
  std::ifstream read(path);
  nlohmann::json j_texts;
  read >> j_texts;
  read.close();

  std::string area = std::filesystem::path(path).stem().string();

  for(auto it=j_texts.begin(); it!=j_texts.end(); it++)
    m_jTexts[it.key()] = {area, it.value()["text"]};

  std::cout << "Created texts with area id: " << area << std::endl;
}
