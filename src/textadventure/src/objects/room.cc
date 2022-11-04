#include "room.h" 
#include "eventmanager/listener.h"
#include "nlohmann/json_fwd.hpp"
#include "person.h"
#include <vector>

#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

//Constructor
CRoom::CRoom(string sArea, nlohmann::json jAtts, std::map<string, CPerson*> 
    characters, std::map<string, CItem*> items, std::map<string, CDetail*> 
    details, CPlayer* p) : CObject(jAtts, p, "room") {
  m_sArea = sArea;
  m_sEntry = jAtts.value("entry", "");

  std::map<std::string, nlohmann::json> mapExits;
  if(jAtts.count("exits") > 0)
    mapExits = jAtts["exits"].get<std::map<std::string, nlohmann::json>>();
  for(const auto &it : mapExits) {
    // Create id (target room)
    std::string id = it.first;
    // Only if the absolute path is not given, add the current area to the 
    // refered room-id.
    if (id.find(".") == std::string::npos)
      id.insert(0, sArea+".");

    // Create exit.
    m_exits[id] = new CExit(id, it.second, p);
  }

  m_characters = characters;
  m_items = items;
  m_details = details; 
}


// *** GETTER *** // 

string CRoom::getEntry() { 
  return m_sEntry; 
}

string CRoom::getArea() { 
  return m_sArea; 
}

std::map<std::string, CPerson*>& CRoom::getCharacters() { 
  return m_characters; 
}

std::map<string, CExit*>& CRoom::getExtits() { 
  return m_exits; 
}

CRoom::objectmap CRoom::getExtits2() { 
  auto lambda = [](CExit* exit) { return exit->name(); };
  return func::convertToObjectmap(m_exits, lambda); 
}

std::map<string, CItem*>& CRoom::getItems() { 
  return m_items; 
}

std::map<string, CDetail*>& CRoom::getDetails() { 
  return m_details; 
}

std::vector<CListener*> CRoom::getHandler() {
  // Add room listeners
  auto listeners = this->listeners();
  // Add character listeners
  for(auto it : m_characters) {
    auto char_listeners = it.second->listeners();
    listeners.insert(listeners.end(), char_listeners.begin(), char_listeners.end());
  }
  return listeners;
} 

// *** SETTER *** //
void CRoom::setPlayers(objectmap& onlinePlayers) { m_players = onlinePlayers; }

void CRoom::setShowMap(nlohmann::json j) {
  for(auto it = j.begin(); it!=j.end(); it++) {
    std::map<string, string> show_mapping = it.value().get<std::map<string, string>>();
    for (const auto& jt : show_mapping)
      if ((jt.first == "pre" && jt.second != "") || (jt.first == "post" && jt.second != "")
          || (jt.first == "also" && jt.second != "") )
        m_showMap[it.key()][jt.first] = jt.second;
    // Check that two keys where added.
    if (m_showMap[it.key()].size() < 2) {
      std::cout << cRED << "invalid show map @key=" << it.key() << cCLEAR << std::endl;
      exit(1);
    }
  }
}


// *** VARIOUS FUNCTIONS *** //

string CRoom::showDescription(std::map<std::string, CPerson*> mapCharacters) {
  string sDesc = "";
  for(auto it : m_characters)
    sDesc += mapCharacters[it.first]->getRoomDescription() + " ";

  if(text_->print().find("</div") != std::string::npos)
    sDesc = "<div class='spoken2'>"+sDesc+"</div>";
  else
    sDesc += "\n";
  return text_->print() + " " + sDesc;
}

string CRoom::showAll(CGramma* gramma) {
  return showExits(gramma) + " " + showCharacters(gramma) + " " 
          + showItems(gramma) + " " + showDetails(gramma) + "\n";
}

string CRoom::showExits(CGramma* gramma) {
  auto lambda = [](CExit* exit) {return (!exit->hidden()) ? exit->prep() + " " + exit->name() : "";};

  // Get pre and post strings from show map.
  std::string pre = (m_showMap.count("exits") > 0) ? m_showMap["exits"]["pre"] : "Hier geht es";    
  std::string post = (m_showMap.count("exits") > 0) ? m_showMap["exits"]["post"] : "Nirgendwo hin.";
  return gramma->build(func::to_vector(m_exits, lambda), pre, post);
}

string CRoom::showCharacters(CGramma* gramma) {

  std::cout << "showCharacters" << std::endl;
  auto lambda = [](CPerson* person) {return person->name();};
  std::string output = "";

  // Get pre and post strings from show map.
  std::string pre = (m_showMap.count("chars") > 0) ? m_showMap["chars"]["pre"] : "Hier sind";    
  std::string post = (m_showMap.count("chars") > 0) ? m_showMap["chars"]["post"] : "niemand.";
  std::string also = (m_showMap.count("chars") > 0) ? m_showMap["chars"]["also"] : "Aber außerdem noch";
  output += gramma->build(func::to_vector(m_characters, lambda), pre, post);
  std::cout << "Got text from gramma: " << output << std::endl;
  if(m_players.size() > 0)
    output += gramma->build(func::to_vector(m_players), " " + also + " ", "");
  std::cout << "Returning..." << std::endl;
  return output;
}       

string CRoom::showItems(CGramma* gramma) {
  string sOutput = "";

  //TODO (fux): condition to not print when hidden might be missing.
  auto printing = [](CItem* item) { return item->name(); };

  return gramma->build(func::to_vector(m_items, printing), "Hier sind", 
      "nichts zu finden.");
} 

string CRoom::showDetails(CGramma* gramma) {
  std::string details = "";
  auto lambda = [](CDetail* detail) { return detail->name(); };
  return gramma->build(func::to_vector(m_details, lambda), "Hier sind ", 
      "nichts weiteres zu sehen.");
}

string CRoom::look(string sDetail, CGramma* gramma) {
  CDetail* detail = m_details[sDetail];
  auto lambda = [](CItem* item) { return item->name(); };
  std::string sOutput = "";

  sOutput = gramma->build(func::to_vector(detail->getItems(), lambda), 
      "Hier sind", "leider nichts.");

  //Change from hidden to visible and "empty" detail
  m_items.insert(detail->getItems().begin(), detail->getItems().end());
  detail->getItems().clear();

  return sOutput;
}

CItem* CRoom::getItem(std::string sPlayerChoice) {
  for (auto it : m_items) {
    if (it.second->getHidden() == true) 
      continue;
    if (fuzzy::fuzzy_cmp(it.second->name(), sPlayerChoice) <= 0.2)
      return it.second;
  }
  return NULL;
}

