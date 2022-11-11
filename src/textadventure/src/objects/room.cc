#include "room.h" 
#include "eventmanager/listener.h"
#include "nlohmann/json_fwd.hpp"
#include "person.h"
#include <memory>
#include <vector>

#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

std::shared_ptr<CGramma> CRoom::gramma_ = nullptr;

#define EXITS "exits"
#define CHARS "chars"
#define ITEMS "items"
#define DETAILS "details"

#define T_S "start" ///< before text f.e. Here are ...
#define T_E "error"  ///< in case list is empty: ending. f.e. ...no things to be found.
#define T_ADD "additional"  ///< Additional parts f.e. ... and also ...

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
    std::cout << "Added " << char_listeners.size() << " listeners from " << it.first << std::endl;
    listeners.insert(listeners.end(), char_listeners.begin(), char_listeners.end());
  }
  return listeners;
} 

// *** SETTER *** //
void CRoom::setPlayers(objectmap& onlinePlayers) { m_players = onlinePlayers; }

void CRoom::setShowMap(nlohmann::json show_config) {
  for (const auto& [type, entries] : show_config.get<std::map<std::string, nlohmann::json>>()) {
    for (const auto& [key, txt] : entries.get<std::map<std::string, std::string>>())
      _show[type][key] = txt;
  }
}

void CRoom::set_gramma(std::shared_ptr<CGramma> gramma) {
  gramma_ = gramma;
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

string CRoom::showAll() {
  return showExits() + " " + showCharacters() + " " + showItems() + " " + showDetails() + "\n";
}

string CRoom::showExits() {
  auto lambda = [](CExit* exit) {return (!exit->hidden()) ? exit->prep() + " " + exit->name() : "";};

  // Get pre and post strings from show map.
  return gramma_->build(func::to_vector(m_exits, lambda), _show[EXITS][T_S], _show[EXITS][T_E]);
}

string CRoom::showCharacters() {
  auto lambda = [](CPerson* person) {return person->name();};
  std::string output = "";

  // Get pre and post strings from show map.
  output += gramma_->build(func::to_vector(m_characters, lambda), _show[CHARS][T_S], _show[CHARS][T_E]);
  if(m_players.size() > 0)
    output += gramma_->build(func::to_vector(m_players), " " + _show[CHARS][T_ADD] + " ", "");
  return output;
}       

string CRoom::showItems() {
  string sOutput = "";

  //TODO (fux): condition to not print when hidden might be missing.
  auto printing = [](CItem* item) { return item->name(); };

  return gramma_->build(func::to_vector(m_items, printing), _show[ITEMS][T_S], _show[ITEMS][T_E]);
} 

string CRoom::showDetails() {
  std::string details = "";
  auto lambda = [](CDetail* detail) { return detail->name(); };
  return gramma_->build(func::to_vector(m_details, lambda), _show[DETAILS][T_S], _show[DETAILS][T_E]);
}

string CRoom::look(string sDetail) {
  CDetail* detail = m_details[sDetail];
  auto lambda = [](CItem* item) { return item->name(); };
  std::string sOutput = "";

  sOutput = gramma_->build(func::to_vector(detail->getItems(), lambda), _show[ITEMS][T_S], 
      _show[ITEMS][T_E]);

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
