#include "room.h" 
#include "person.h"

//Constructor
CRoom::CRoom(string sArea, nlohmann::json jAtts, std::map<string, CPerson*> 
    characters, std::map<string, CItem*> items, std::map<string, CDetail*> 
    details, CPlayer* p) : CObject{jAtts, p} {
  m_sArea = sArea;
  m_sEntry = jAtts.value("entry", "");

  std::map<std::string, nlohmann::json> mapExits;
  if(jAtts.count("exits") > 0)
    mapExits = jAtts["exits"].get<std::map<std::string, nlohmann::json>>();
  for(const auto &it : mapExits) {
    //Create id (target room)
    std::string sID = it.first;
    if(it.second.count("area") > 0)
        sID.insert(0, it.second["area"].get<std::string>() + "_");
    else
        sID.insert(0, sArea+"_");

    //Create exit.
    m_exits[sID] = new CExit(sID, it.second, p);
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
  auto lambda = [](CExit* exit) { return exit->getName(); };
  return func::convertToObjectmap(m_exits, lambda); 
}

std::map<string, CItem*>& CRoom::getItems() { 
  return m_items; 
}

std::map<string, CDetail*>& CRoom::getDetails() { 
  return m_details; 
}

std::vector<nlohmann::json> CRoom::getHandler() {
  std::vector<nlohmann::json> handler;
  handler.insert(handler.begin(), m_handler.begin(), m_handler.end());
  for(auto it : m_characters) {
    std::vector<nlohmann::json> c_handler = it.second->getHandler();
    for(auto &jt : c_handler) {
      if(jt.count("infos") > 0)
        jt["infos"] = it.first;
    }
    handler.insert(handler.end(), c_handler.begin(), c_handler.end());
  }
  return handler;
} 

// *** SETTER *** //
void CRoom::setPlayers(objectmap& onlinePlayers) { m_players = onlinePlayers; }
void CRoom::setShowMap(nlohmann::json j) {
  for(auto it = j.begin(); it!=j.end(); it++)
    m_showMap[it.key()] = it.value().get<std::vector<string>>();
}


// *** VARIOUS FUNCTIONS *** //

string CRoom::showDescription(std::map<std::string, CPerson*> mapCharacters) {
  string sDesc = "";
  for(auto it : m_characters)
    sDesc += mapCharacters[it.first]->getRoomDescription() + " ";

  if(m_text->print().find("</div") != std::string::npos)
    sDesc = "<div class='spoken2'>"+sDesc+"</div>";
  else
    sDesc += "\n";
  return m_text->print() + " " + sDesc;
}

string CRoom::showAll(CGramma* gramma) {
  return showExits(gramma) + " " + showCharacters(gramma) + " " 
          + showItems(gramma) + " " + showDetails(gramma) + "\n";
}

string CRoom::showExits(CGramma* gramma) {
  auto lambda = [](CExit* exit) {return exit->getPrep() + " " + exit->getName();};
  std::string pre = "Hier geht es";    
  std::string post = "Nirgendwo hin.";
  if(m_showMap.count("exits") > 0) {
    if(m_showMap["exits"].size() == 2) {
        pre = m_showMap["exits"][0];
        post = m_showMap["exits"][1];
    }
    else
        std::cout << "Wrong size!\n";
  }
  return gramma->build(func::to_vector(m_exits, lambda), pre, post);
}

string CRoom::showCharacters(CGramma* gramma) {
  auto lambda = [](CPerson* person) {return person->getName();};
  std::string sOutput = "";
  if(m_characters.size() > 0) {
    sOutput += gramma->build(func::to_vector(m_characters, lambda), 
        "Hier sind", "niemand");
  }
  if(m_players.size() > 0)
    sOutput += gramma->build(func::to_vector(m_players), " Aber außerdem noch ", "");
  if(sOutput == "")
    sOutput = "In diesem Raum sind keine Personen.";
  return sOutput;
}       

string CRoom::showItems(CGramma* gramma) {
  string sOutput = "";

  //TODO (fux): condition to not print when hidden might be missing.
  auto printing = [](CItem* item) { return item->getName(); };

  return gramma->build(func::to_vector(m_items, printing), "Hier sind", 
      "nichts zu finden.");
} 

string CRoom::showDetails(CGramma* gramma) {
  std::string details = "";
  auto lambda = [](CDetail* detail) { return detail->getName(); };
  return gramma->build(func::to_vector(m_details, lambda), "Hier sind ", 
      "nichts weiteres zu sehen.");
}

string CRoom::look(string sDetail, CGramma* gramma) {
  CDetail* detail = m_details[sDetail];
  auto lambda = [](CItem* item) { return item->getName(); };
  std::string sOutput = "";

  sOutput = gramma->build(func::to_vector(detail->getItems(), lambda), 
      "Hier sind", "leider nichts.");

  //Change from hidden to visible and "empty" detail
  m_items.insert(detail->getItems().begin(), detail->getItems().end());
  detail->getItems().clear();

  return sOutput;
}

CItem* CRoom::getItem(std::string sPlayerChoice) {
  for(auto it : m_items) {
    if(it.second->getHidden() == true) 
      continue;
    if(fuzzy::fuzzy_cmp(it.second->getName(), sPlayerChoice) <= 0.2)
      return it.second;
  }
  return NULL;
}

