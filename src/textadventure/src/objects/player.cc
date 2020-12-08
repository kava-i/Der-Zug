#include "player.h"

#define BLACK Webcmd::set_color(Webcmd::color::BLACK)
#define GREEN Webcmd::set_color(Webcmd::color::GREEN)
#define RED Webcmd::set_color(Webcmd::color::RED)
#define BLUE Webcmd::set_color(Webcmd::color::BLUE)
#define PURPLE Webcmd::set_color(Webcmd::color::PURPLE)
#define WHITE Webcmd::set_color(Webcmd::color::WHITE)
#define WHITEDARK Webcmd::set_color(Webcmd::color::WHITEDARK)
#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

/**
* Full constructor for player
* @param jAtts json with all attributes
* @param room current room of payer
* @param newAttacks attacks of player
*/
CPlayer::CPlayer(nlohmann::json jAtts, CRoom* room, attacks lAttacks, 
    CGramma* gramma, std::string path) : CPerson(jAtts, nullptr, lAttacks, nullptr, this, 
      std::map<std::string, CDialog*>()) {
  //Set login data and player information
  func::convertToUpper(m_sName);
  m_sPassword = jAtts["password"];
  m_firstLogin = true; 
  m_abbilities = {"strength", "skill"};

  //Initiazize world
  m_world = new CWorld(this, path);
  m_parser = new CParser(m_world->getConfig());
  m_gramma = gramma;
  
  //Character and Level
  m_level = 0;
  m_ep = 0;
  
  //Extract minds from config
  std::map<std::string, std::string> colors = {{"blue", BLUE}, 
    {"green", GREEN}, {"red", RED}};
  for(auto it : m_world->getConfig()["minds"]) {
    SMind mind;
    mind.sID = it["id"];
    mind.color = colors[it["color"]];
    mind.level = it["level"].get<int>();
    mind.relevance = it["relevance"].get<double>();
    m_minds[it["id"]] = mind;
  }

  //Attributes
  if(m_world->getConfig().count("attributes")>0) {
    std::vector<std::string> attributes = m_world->getConfig()["attributes"];
    for(auto it : attributes)   
        m_stats[it] = 0;
  }

  //States, f.e. current fight, Dialog-partner
  m_curFight = nullptr;
  m_curDialogPartner = nullptr;

  //Set current room
  m_room = room;

  //Set player's equipment (no equipment at the beginning)
  m_equipment["weapon"] = NULL;
  m_equipment["armor"]  = NULL;
  
  //Initialize all rooms as not visited
  for(const auto& it : m_world->getRooms())
    m_vistited[it.second->getID()] = false;
  m_room = m_world->getRoom(m_room->getID());
  m_vistited[m_room->getID()] = true;

  std::cout << "initializing context stack.\n";
  //Initialize context stack
  for(auto it : m_world->getQuests()) {
    if(it.second->getOnlineFromBeginning() == true) {
      Context* context = new Context(
          (nlohmann::json){{"name", it.first}, {"permeable",true}, 
          {"questID",it.first}});
      context->initializeHandlers(it.second->getHandler());
      m_contextStack.insert(context, 3, it.first);
    }
  }
  std::cout << "Done.\n";

  //Add eventhandler to eventmanager
  m_contextStack.insert(new Context((std::string)"first"), 9, "first");
  m_contextStack.insert(new Context((std::string)"world"), 2, "world");
  m_contextStack.insert(new Context((std::string)"standard"), 0, "standard");
  updateRoomContext();

  //Add quests
  if(jAtts.count("quests") > 0) {
    for(const auto& it : jAtts["quests"])
      setNewQuest(it);
  }
}

CPlayer::~CPlayer() {
  delete m_world;
} 

// *** GETTER *** // 

bool CPlayer::getFirstLogin() { 
  return m_firstLogin; 
}

string CPlayer::getPrint()  { 
  if (m_staged_post_events != "")
    throw_staged_events(m_staged_post_events, "post");
  checkCommands();
  return m_sPrint + "\n";
}

CWorld* CPlayer::getWorld() { 
  return m_world; 
}

CGramma* CPlayer::getGramma() {
  return m_gramma;
}

CRoom* CPlayer::getRoom() { 
  return m_room; 
}

std::map<std::string, SMind>& CPlayer::getMinds() { 
  return m_minds; 
}

SMind& CPlayer::getMind(std::string sMind) {
  if(m_minds.count(func::returnToLower(sMind)) > 0)
      return m_minds[func::returnToLower(sMind)];
  std::cout << cRED << "FATAL Error! Requested mind does not exist: " 
    << sMind << std::endl;
  return m_minds["logik"]; 
}

std::vector<std::string> CPlayer::getAbbilities() { 
  return m_abbilities; 
}

std::map<std::string, CItem*>& CPlayer::getEquipment() { 
  return m_equipment; 
}

CFight* CPlayer::getFight() { 
  return m_curFight; 
}

CPerson* CPlayer::getCurDialogPartner() {
  return m_curDialogPartner;
}

CContextStack<Context>& CPlayer::getContexts() { 
  return m_contextStack; 
}

Context* CPlayer::getContext(std::string context) {
  if(m_contextStack.getContext(context) != NULL)
    return m_contextStack.getContext(context);
  std::cout << cRED << "FATAL!!! Context not found: " << context << cCLEAR << std::endl;
  return nullptr;
}


std::map<std::string, CPlayer*>& CPlayer::getMapOFOnlinePlayers() {
  return m_players;
}

std::map<std::string, std::string> CPlayer::GetCurrentStatus() {
  //Setup basic elements
  std::map<std::string, std::string> status = {{"room", (std::string)m_room
    ->getID()}, {"inventory", (std::string)m_inventory.getItemList()}};

  //Add extra substitutes
  status.insert(subsitutes_.begin(), subsitutes_.end());

  //Get highest mind
  int max = 0;
  std::string max_mind = "";
  std::string max_minds = "";
  for (auto it : m_minds) {
    if (it.second.level > max) {
      max = it.second.level;
      max_mind = it.first;
      max_minds = it.first;
    }
    if (it.second.level == max)
      max_minds+=";"+it.first;
  }
  status["max_mind"] = max_mind;
  status["max_minds"] = max_minds;


  //Add attributes
  auto lambda1 = [](int x) { return std::to_string(x); };
  std::map<std::string, std::string> attributes = 
      func::convertToObjectmap(m_stats, lambda1);
  status.insert(attributes.begin(), attributes.end());

  //Add minds
  auto lambda2 = [](SMind mind) { return std::to_string(mind.level); };
  std::map<std::string, std::string> minds = 
      func::convertToObjectmap(m_minds, lambda2);
  status.insert(minds.begin(), minds.end());

  return status;
}

// *** SETTER *** // 

void CPlayer::setFirstLogin(bool val) { 
  m_firstLogin = val; 
}

void CPlayer::setPrint(string newPrint) { 
  m_sPrint = newPrint; 
}

void CPlayer::set_subsitues(std::map<std::string, std::string> subsitutes) {
  subsitutes_ = subsitutes;
}

void CPlayer::printText(std::string text) {
  if (m_world->getTexts().count(text) > 0) {
    nlohmann::json j_text = m_world->getTexts().at(text);
    CText text(j_text, this);
    m_sPrint += text.print();
  }
  else
    std::cout << cRED << "Text not found!" << cCLEAR << std::endl;
}

void CPlayer::appendPrint(std::string sPrint) {
  throw_staged_events(m_staged_pre_events, "pre");
  if(m_staged_pre_events != "")
    m_sPrint += "\n";
  m_sPrint += sPrint;
  throw_staged_events(m_staged_post_events, "post"); 
}

void CPlayer::appendStoryPrint(string sPrint) { 
  appendSpeackerPrint(m_world->getConfig()["printing"].value("story", ""), 
      sPrint);
}

void CPlayer::appendDescPrint(string sPrint) {
  appendSpeackerPrint(m_world->getConfig()["printing"].value("desc", ""), 
      sPrint);
}

void CPlayer::appendErrorPrint(string sPrint) {
  appendSpeackerPrint(m_world->getConfig()["printing"].value("error", ""), 
      sPrint);
}

void CPlayer::appendTechPrint(string sPrint) {
  appendSpeackerPrint(m_world->getConfig()["printing"].value("tech", "")
      , sPrint);
}

void CPlayer::appendBlackPrint(std::string sPrint) {
  appendPrint(returnBlackPrint(sPrint));
}

void CPlayer::appendSpeackerPrint(std::string sSpeaker, std::string sPrint) {
  appendPrint(returnSpeakerPrint(sSpeaker, sPrint));
}

std::string CPlayer::returnSpeakerPrint(std::string sSpeaker, std::string sPrint) {
  sPrint = func::returnSwapedString(sPrint, getStat("highness"));
  if(sSpeaker != "")
    return "<div class='spoken'>" + sSpeaker + " - " + WHITEDARK + sPrint 
      + WHITE + "</div>";
  return sPrint + "\n";
}

std::string CPlayer::returnBlackPrint(std::string sPrint) {
  return "<div class='spoken2'>" + WHITEDARK + sPrint + WHITE + "</div>";
}

void CPlayer::appendSuccPrint(string sPrint) {
  appendPrint(GREEN + sPrint + WHITE);
}

void CPlayer::addPreEvent(std::string sNewEvent) {
  if(m_staged_pre_events == "")
    m_staged_pre_events = sNewEvent;
  else
    m_staged_pre_events += ";"+sNewEvent;
}

void CPlayer::addPostEvent(std::string sNewEvent) {
  if(m_staged_post_events == "")
    m_staged_post_events = sNewEvent;
  else
    m_staged_post_events += ";"+sNewEvent;
}

//Throw staged events and clear events afterwards
void CPlayer::throw_staged_events(std::string& events, std::string sMessage)  {
  if(events == "") 
    return;
  std::string newEvents = events;
  events = "";
  throw_events(newEvents, sMessage+"-events"); 
}


///Set player's world.
void CPlayer::setWorld(CWorld* newWorld) { 
    m_world = newWorld; 
}

///Set last room to current room and current room to new room.
void CPlayer::setRoom(CRoom* room) { 
    m_lastRoom = m_room; m_room = room; 
}

///Set map of all online players.
void CPlayer::setPlayers(map<string, CPlayer*> players) { 
    m_players = players; 
}

///Set webconsole.
void CPlayer::setWebconsole(Webconsole* webconsole) { 
  _cout = webconsole; 
}


// *** *** FUNCTIONS *** *** // 


/**
* Update room context after changing location
*/
void CPlayer::updateRoomContext() {
  m_room->setShowMap(m_world->getConfig()["show"]);

  //Create new room context
  Context* context = new Context((std::string)"room");

  //Transfer Time events if context exists
  if(m_contextStack.getContext("room") != NULL)
    context->setTimeEvents(m_contextStack.getContext("room")->getTimeEvents());

  //Delete old room-context
  m_contextStack.erase("room");

  //Update handler
  for(auto it : m_room->getHandler()) {
    context->add_listener(it);
    if(it.count("infos") > 0)
      context->getAttributes()["infos"][(std::string)it["id"]] = it["infos"];
  }
  
  //Insert new room-context into context-stack
  m_contextStack.insert(context, 0, "room");
}

// *** Fight *** //

/**
* Set current fight of player and add a fight-context to context-stack
* @param newFight new fight
*/
void CPlayer::setFight(CFight* newFight) { 
    m_curFight = newFight;

    //Create fight-context and add to context-stack.
    Context* context = new Context((std::string)"fight");
    context->initializeFightListeners(getAttacks2());
    m_contextStack.insert(context, 1, "fight");

    m_curFight->initializeFight();
}

/**
* Delete the current fight and erase fight-context from context-stack
*/
void CPlayer::endFight() {
    delete m_curFight;
    m_contextStack.erase("fight");
    appendPrint("Fight ended.\n");
}

// *** Dialog *** //


/**
* Set current (new) Dialog player and add a Dialog-context to context-stack.
* @param sCharacter id of dialogpartner and throw event to start Dialog.
*/
void CPlayer::startDialog(string sCharacter, CDialog* dialog) {
  //Set current dialog
  if(dialog != nullptr)
    m_dialog = dialog;
  else
    m_dialog = m_world->getCharacter(sCharacter)->getDialog();

  //Add person to current dialog partner
  m_curDialogPartner = m_world->getCharacter(sCharacter);       

  //Create context and add to context-stack.
  Context* context = new Context((std::string)"dialog", {{"partner", sCharacter}});
  context->initializeDialogListeners("START", this);
  m_contextStack.insert(context, 1, "dialog");

  std::string newCommand = m_dialog->getState("START")->callState(this);
  if(newCommand != "")
    throw_events(newCommand, "CPlayer::startDialog");
}

/**
* Try to start chatting. If player is busy, print error message, else add chat-context to 
* context-stack and throw event 'Hey + player-name'.
*/
void CPlayer::startChat(CPlayer* player) {
  appendStoryPrint("Du gehst auf " + player->getName() + 
      " zu und räusperst dich...\n");

  if(player->getContexts().nonPermeableContextInList() == true)
    appendStoryPrint(player->getName() + " ist zur Zeit beschäftigt.\n");
  else { 
    //Add Chat context for both players
    addChatContext(player->getID());
    player->addChatContext(m_sID);
    
    //Send text by throwing event
    throw_events("Hey " + player->getName() + ".", "CPlayer::startChat");
  }
}

/**
* Add chat context.
* @param sPlayer (chat partner (other player))
*/
void CPlayer::addChatContext(std::string sPartner) {
  Context* context = new Context("chat", {{"partner", sPartner}});
  context->add_listener("h_send", (std::regex)"(.*)", 1);
  m_contextStack.insert(context, 1, "chat");
}


/**
* Direct print of message to web-console. 
* Used when chatting and usually call in chat-context
* of the Dialog partner, printing, what he said.
* @param sMessage message to print to console
*/
void CPlayer::send(string sMessage) {
  _cout->write(sMessage);
  _cout->flush(); 
}

/**
* Add read context.
* @param sItem (id of book which to read)
*/
void CPlayer::addReadContext(std::string sID) {
  //Create context and add to context-stack.
  CItem* item = m_inventory.getItem_byID(sID);
  Context* context = new Context((std::string)"read", 
                                  {{"mark", item->getMark()},{"item",sID}});
  m_contextStack.insert(context, 1, "read");
}


// *** Room *** 

/**
* Check player input, whether 1. player wants to go back, 2. player wants 
* to use an exit in current
* room, or 3. player wants to go to a room already visited.
* @param sIdentifier player input (room id, exit of current room, or room al-
* redy visited)
*/
void CPlayer::changeRoom(string sIdentifier) {
  //Check if player wants to go back.
  if(sIdentifier == "back" || sIdentifier == "zurück") {
    changeRoom(m_lastRoom);
    return;
  }

  //Get selected room, checking exits in current room.
  auto lamda1= [](CExit* exit) { return exit->getPrep() + " " + exit->getName(); };
  string room = func::getObjectId(getRoom()->getExtits(), sIdentifier, lamda1);

  if(room != "") {
    changeRoom(getWorld()->getRooms()[room]);
    return;
  }

  //Check all rooms already visited.
  auto lamda2 = [](CRoom* room) { return room->getName(); };
  room = func::getObjectId(m_world->getRooms(), sIdentifier, lamda2);
  std::vector<std::string> path = findWay(m_room, room);

  //If a path was found, add events to go to each room in path.
  if(path.size() > 0) {
    std::string events;
    for(const auto& it : path)
      events += "go " + it + ";";
    events.pop_back();
    throw_events(events, "CPlayer::changeRoom");
  }
  
  //Print error message.
  else
    appendErrorPrint("Room not found.\n");
}

/**
* Change room to given room and print entry description. Set last room to current room.
* @param newRoom new room the player changes to
*/
void CPlayer::changeRoom(CRoom* newRoom) {
  m_lastRoom = m_room; 
  m_room = newRoom;
  std::string entry = newRoom->getEntry();
  if(entry != "")
    appendDescPrint(entry);
  appendPrint(m_room->showDescription(m_world->getCharacters()));
  m_vistited[m_room->getID()] = true;

  updateRoomContext();
  std::cout << "done.\n";
}

/**
* Print all already visited rooms.
*/
void CPlayer::showVisited() {
  std::map<std::string, std::string> mapRooms;
  for(auto const& it : m_vistited) {
    if(it.second == true)
      mapRooms[it.first] = m_world->getRoom(it.first)->getName();
  }
  appendDescPrint(m_gramma->build(func::to_vector(mapRooms), "Du warst schon in",
        "keinem anderen Raum.") + "\nGebe \"gehe [name des Raumes]\" ein, um "
        "direkt dort hinzugelangen.\n");
}


/**
* Look for shortes way (if exists) to given room. And return way as vector, or empty array.
* @param room players room
* @param roomID id of desired target-room
* @return vector with way to target.
*/
std::vector<std::string> CPlayer::findWay(CRoom* room, std::string roomID) {
  //Check whether current room is target room
  if(room->getID() == roomID)
    return {};

  //Set variables
  std::queue<CRoom*> q;
  std::map<std::string, std::string> parents;

  //Set parent of every room to ""
  for(auto it : m_world->getRooms())
    parents[it.second->getID()] = "";

  //Normal breadth-first search
  q.push(room);
  parents[room->getID()] = room->getID();
  while(!q.empty()) {
    CRoom* node = q.front(); 
    q.pop();
    if(node->getID() == roomID)
      break;
    for(auto& it : node->getExtits()) {
      //Also check, that target-room is visited and in the same area as current room.
      if(parents[it.first] == "" && m_vistited[it.first] == true 
          && node->getArea() == m_room->getArea()) {
        q.push(m_world->getRoom(it.first));
        parents[it.first] = node->getID();
      }
    } 
  }

  //Check whether rooom has been found, if not, return empty array.
  if(parents[roomID] == "") 
    return {};

  //Create path from parents.  
  std::vector<std::string> path = { roomID };
  while(path.back() != room->getID())
    path.push_back(parents[path.back()]);
  path.pop_back();

  //Reverse path and return.
  std::reverse(std::begin(path), std::end(path));
  return path;
}


// *** Item and Inventory *** //

/**
* Adding all (non-hidden) items in room to players inventory.
*/
void CPlayer::addAll() {
  std::vector<std::string> items_names;
  for(auto it = m_room->getItems().begin(); it != m_room->getItems().end();) {
    if((*it).second->getHidden() == false)  {
      m_inventory.addItem((*it).second);
      items_names.push_back((*it).second->getName());
      m_room->getItems().erase((*(it++)).second->getID());
      continue;
    }
    ++it;
  }
  appendDescPrint(m_gramma->build(items_names, "Du findest", "keine Gegenstände."));
}

/**
* Add given item to player's inventory and print message.
* @param item given item/ item to add to inventory.
*/
void CPlayer::addItem(CItem* item) {
  m_inventory.addItem(item);
  appendDescPrint("<b> "+item->getName() + "</b> zu {name}'s Inventar hinzugefügt.\n");
  m_room->getItems().erase(item->getID());
}

/**
* Start a trade. Add a trade-context to player's context stack.
* @param partner Player's trading partner add to context-information.
*/
void CPlayer::startTrade(std::string partner) {
  appendDescPrint("Started to trade with " + partner + ".\n");

  //Create trade-context and add to context-stack
  Context* context = new Context((std::string)"trade", {{"partner", partner}});
  context->print(this);
  m_contextStack.insert(context, 1, "trade");
}

/**
* Add all equipped items to player's output.
*/
void CPlayer::printEquiped() {
    auto getElem = [](CItem* item){ 
      std::string str; 
      if(item) 
        str = item->getName(); 
      else 
        str="nichts ausgerüstet.";
      return str;
    };
    appendPrint(func::table(m_equipment, getElem, "width:20%"));
}

/**
* Equipe given item into given category (sType). If there is already an item equipped in this 
* category, add choice-context to context-stack.
* @param item given item.
* @param sType type of item indicating category.
*/
void CPlayer::equipeItem(CItem* item, string sType) {
  //If nothing is equipped in this category -> equip.
  if(m_equipment[sType] == NULL) {
    appendDescPrint("Du hast " + item->getName() + " als " + sType + " ausgerüstet.\n");
    string sAttack = item->getAttack();

    //Check for new attack
    if(sAttack != "") {
      m_attacks[sAttack] = m_world->getAttack(sAttack);
      appendDescPrint("Neue Attacke: \"" + m_attacks[sAttack]->getName() + "\".\n");
    }
    m_equipment[sType] = item;
  }

  //If this item is already equipped -> print error message.
  else if(m_equipment[sType]->getID() == item->getID())
    appendErrorPrint(sType + " bereits ausgerüstet.\n");

  //If another item is equipped in this category -> add choice-context
  else {
    appendErrorPrint("Bereits ein " + sType + " ausgerüstet. Austauschen? (ja/nein)\n");

    //Create Choice-Context
    m_contextStack.insert(new Context((nlohmann::json){{"name", "equipe"}, 
          {"permeable", false},{"error", "Wähle ja oder nein\n"},{"itemID", 
          item->getID()},{"handlers",{{"ja",{ "h_choose_equipe"}},{"nein",
          {"h_choose_equipe"}}}}}), 1, "equipe");
  }
}

/**
* Dequipe an item. Erase attack from list of attacks if it depended on equipped item.
* @param sType item type (weapon, clothing etc.)
*/
void CPlayer::dequipeItem(string sType) {
  if(m_equipment.count(sType) == 0)
    appendErrorPrint("Nothing to dequipe.\n");
  else if(m_equipment[sType] == NULL)
    appendErrorPrint("Nothing to dequipe.\n");
  else {
    appendDescPrint(sType + " " + m_equipment[sType]->getName() + " abgelegt.\n");

    //Erase attack
    if(m_equipment[sType]->getAttack() != "")
      m_attacks.erase(m_equipment[sType]->getAttack());
    m_equipment[sType] = NULL;
  }
}

// *** QUESTS *** //

/**
* show all active or solved quest depending on 'solved'
* @param solved indicating, whether to show solved or active quests.
*/
void CPlayer::showQuests(bool solved) {
  if(solved == false)
    m_sPrint += "Active quests: \n";
  else
    m_sPrint += "Solved quests: \n";

  for(auto it : m_world->getQuests()) {
    if(it.second->getActive() == true)
      m_sPrint += "\n" + it.second->printQuest(solved);
  }
}

/**
* Add new quest by setting quest active.
* @param sQuestID id to given quest.
*/
void CPlayer::setNewQuest(std::string sQuestID) {
  int ep=0;
  CQuest* quest = m_world->getQuest(sQuestID);
  appendSuccPrint(quest->setActive(ep, this));
  if(quest->getOnlineFromBeginning() == false) {
    Context* context = new Context((nlohmann::json){{"name", sQuestID}, 
        {"permeable",true}, {"questID",sQuestID}});
    context->initializeHandlers(quest->getHandler());
    m_contextStack.insert(context, 3, sQuestID);
  }
  
  if(quest->getSolved() == true)
    m_contextStack.erase(sQuestID);
  addEP(ep);
}

/**
* Set a quest-step as solved and add received ep (experience points) to players ep.
* @param sQuestID identifier to quest.
* @param sStepID identifier to quest-step.
*/
void CPlayer::questSolved(std::string sQuestID, std::string sStepID) {
  int ep=0;
  m_world->getQuest(sQuestID)->getSteps()[sStepID]->solved(ep, this);
  if(m_world->getQuest(sQuestID)->getSolved() == true)
      m_contextStack.erase(sQuestID);
  addEP(ep);
}


// *** Minds and Level *** //

/**
* Add experience points and call update-stats function if a new level is reached.
* @param ep experience points to be added.
*/
void CPlayer::addEP(int ep) {
  m_ep+=ep;
  size_t counter=0;
  for (;m_ep>=20; m_ep-=20, counter++, m_level++)
    appendSuccPrint("Level Up!\n");

  if (counter > 0) {
    Context* context = m_contextStack.getContext("updateStats");
    if (context == NULL)
      updateStats(counter);
    else {
      context->setAttribute<int>("numPoints", context->getAttribute<int>
          ("numPoints")+counter);
      m_sPrint += "Du hast " + std::to_string(context->getAttribute<int>
          ("numPoints")) + " Punkte zu vergeben!\n";
      for(size_t i=0; i<m_abbilities.size(); i++) {
          m_sPrint += std::to_string(i+1) + ". " + m_abbilities[i] + ": level(" 
            + std::to_string(getStat(m_abbilities[i])) + ")\n";
      }
      m_sPrint += "Wähle eine Zahl geben den Namen des Attributes aus.\n";
    }
  }
}

/**
* Let player know how many learning points player can assign and add choice context.
* @param numPoints experience points player can assign.
*/
void CPlayer::updateStats(int numPoints)
{
    m_sPrint+= "Du hast " + std::to_string(numPoints) + " Punkte zu vergeben.\n";

    //Print attributes and level of each attribute and add choice-context.
    std::string sError = "Wähle eine Zahl oder den Namen des Attributes aus.\n";
    Context* context = new Context((nlohmann::json){{"name", "updateStats"}, {"permeable", false},{"numPoints", numPoints}, {"error", sError}});

    for(size_t i=0; i<m_abbilities.size(); i++)
        m_sPrint += std::to_string(i+1) + ". " + m_abbilities[i] + ": level(" + std::to_string(getStat(m_abbilities[i])) + ")\n";

    
    context->add_listener("h_updateStats", m_abbilities);

    m_sPrint += sError;
    m_contextStack.insert(context, 1, "updateStats");
}

/**
* Print minds of player by using table function.
*/
void CPlayer::showMinds()
{
    m_sPrint += " --- " + m_sName + " --- \n"
            += "Level: " + std::to_string(m_level) + "\n"
            += "Ep: " + std::to_string(m_ep) + "/20.\n";

    auto lamda = [](SMind mind) { return std::to_string(mind.level);};
    m_sPrint += func::table(m_minds, lamda);
}

/**
* Print player's stats by using table function.
*/
void CPlayer::showStats() {

    m_sPrint += "--- " + m_sName + "---\n";
    
    std::map<std::string, std::map<std::string, std::string>> mapStats = m_world->getConfig()["mapAttributes"];
    std::map<std::string, int> mStats;
    for(auto it : m_stats)
    {
        if(mapStats.count(it.first) == 0)
            mStats[it.first] = it.second;
    }
    auto lambda = [](int stat) { return std::to_string(stat); };
    m_sPrint += func::table(mStats, lambda);
    for(auto it : m_stats)
    {
        if(mapStats.count(it.first) > 0)
            m_sPrint += mapStats[it.first][std::to_string(it.second)] + "\n";
    }
}

/**
* Check given dependencies. Receive a json and check whether this matches player's 
* minds or stats.
* @param jDeps json with dependencies
*/
bool CPlayer::checkDependencies(nlohmann::json jDeps) {
  std::map<std::string , std::function<bool(int, int)>> operators;
  operators["="] = [](int a, int b) { return a=b; };
  operators[">"] = [](int a, int b) { return a>b; };
  operators[">="] = [](int a, int b) { return a>=b; };
  operators["<"] = [](int a, int b) { return a<b; };
  operators["<="] = [](int a, int b) { return a<=b; };
  operators["!="] = [](int a, int b) { return a!=b; };
  operators["&"] = [](int a, int b) { return true; };

  if(jDeps.size() == 0)
    return true;
  for(auto it=jDeps.begin(); it!=jDeps.end(); it++) {
    //Assign value with position 0 = operator
    std::string sOpt = func::extractLeadingChars(it.value());
    int value = func::getNumFromBack(it.value());

    if(sOpt == "&") {
      double level = m_minds[it.key()].level + m_minds[it.key()].relevance;
      for(const auto& mind : m_minds) {
        if(mind.first != it.key() && mind.second.level + mind.second.relevance > level)
          return false;
      }
    }

    //Check dependency in mind
    if(m_minds.count(it.key()) > 0) {
      if(operators[sOpt](m_minds[it.key()].level, value) == false)
        return false;
    }

    //Check dependency in stats
    else if(m_stats.count(it.key()) > 0) {
      if(operators[sOpt](m_stats[it.key()], value) == false)
        return false;
    }
    else {
      std::cout << cRED << "Error in document: " << it.key() << cCLEAR << std::endl;
      return false;
    }
  } 

  return true;
}


// *** Others *** // 

/**
* Check if player's output contains special commands such as printing player name or else.
*/ 
void CPlayer::checkCommands()
{
    while(m_sPrint.find("{") != std::string::npos)
    {
        size_t pos  = m_sPrint.find("{");
        size_t pos2 = m_sPrint.find("}"); 
        std::string cmd = m_sPrint.substr(pos+1, pos2-(pos+1));
        std::string replace = "";
        if(cmd.find("cname") != std::string::npos && m_curDialogPartner != nullptr)
            replace = m_curDialogPartner->getName();
        else if(cmd.find("name") != std::string::npos)
            replace = getName();

        m_sPrint = m_sPrint.substr(0, pos) + replace + m_sPrint.substr(pos2+1);
    }

    for(const auto& it : m_minds) 
    {
        size_t pos=0;
        for(;;)
        {
            if(m_sPrint.find(func::returnToUpper(it.first), pos) == std::string::npos)
                break;
            pos=m_sPrint.find(func::returnToUpper(it.first), pos);
            m_sPrint.insert(pos, it.second.color);
            m_sPrint.insert(pos + it.first.length()+1, WHITE);
            pos+=it.first.length();
        }
    }

    size_t pos = m_sPrint.rfind("$");
    if(pos != std::string::npos)
        m_sPrint.erase(pos, 1);
}

/**
* Check whether given password and name matches this player.
* @param sName name to compare to player's name.
* @param sPassword password to compare to player's password.
*/
string CPlayer::doLogin(string id, string password) {
  if(id == func::returnToLower(m_sID) && password== m_sPassword) 
    return m_sID;
  else 
    return "";
}

/**
* Get a player from currently online player's by their name using fuzzy comparison. 
* @param sIdentifier identifier (player's name)
*/
CPlayer* CPlayer::getPlayer(string sIdentifier)
{
    //Try to get player's id by using function from func.h
    auto lambda = [](CPlayer* player) { return player->getName(); };
    std::string sPlayerID = func::getObjectId(m_players, sIdentifier, lambda);

    //Check if player's id could be retrieved.
    if(sPlayerID != "")
        return m_players[sPlayerID];

    //If not, print error message on console and for player and return null-pointer.
    printError("FATAL ERROR!!! Player accessed that does not exist: " + sIdentifier);
    return nullptr;
}

/**
* Add a select-context to context-stack.
* @param mapObjects map of objects from which to select.
* @param sEventType type of event.
*/
void CPlayer::addSelectContest(std::map<std::string, std::string> mapObjects, std::string sEventType)
{
    //Add context.
    Context* context = new Context((nlohmann::json){{"name", "select"}, {"permeable",true},{"eventtype",sEventType}, {"map_objects",mapObjects}});

    //Set error function, to delete context when not called 
    context->setErrorFunction(&Context::error_delete); 

    //Add listeners/ eventhandlers
    context->add_listener("h_select", mapObjects);
    
    //Insert context into context-stack.
    m_contextStack.insert(context, 1, "select");
}

/**
* In the case of a serious error, leading game not not continue, let the player no
* that something went wrong and print error in console.
*/
void CPlayer::printError(std::string sError)
{
    std::cout << "\033[1;31m"+sError+"\033[0m"<< std::endl;
    appendTechPrint("Sorry, but something went wrong. Maybe try something else.\n");
}


// *** Eventmanager functions *** //

/**
* Throw event. This is the key function in the program. The parser first parses the command
* into an event which will then be thrown. An event consists of and event type 
* and an identifier indicating what will happen. For example "show people", where "show"
* is the event type and "people" tells the event handler what to do. (similar: "go to foyer").
* this event will be send through all contexts. Each context has a list of handlers. If the 
* event type matches with a handler a function is triggered. If a context is not permeable the 
* loop breaks.
* @param sInput
*/
void CPlayer::throw_events(string sInput, std::string sMessage) {
  updateRoomContext();
  std::cout << cRED << "Events: " << sInput << ", from: " << sMessage 
    << cCLEAR << std::endl;

  //Check for time triggered events
  getContext("room")->throw_timeEvents(this);
  getContext("standard")->throw_timeEvents(this);

  if (sInput == "") {
    throw_staged_events(m_staged_pre_events, "pre");
    throw_staged_events(m_staged_post_events, "post"); 
    return;
  }

  //Parse command
  std::vector<std::pair<std::string, std::string>> events = m_parser->parse(sInput);

  //Iterate over parsed events and call throw_event for each context and each event
  std::deque<Context*> sortedCtxList = m_contextStack.getSortedCtxList();
  for (size_t i=0; i<events.size(); i++) {
    std::cout << cRED << events[i].first << ", " << events[i].second << cCLEAR <<"\n";

    //Check for non-event type commands
    if (events[i].first == "printText") 
      printText(events[i].second);
    else {
      for (size_t j=0; j<sortedCtxList.size(); j++) {
        if (sortedCtxList[j]->throw_event(events[i], this) == false)
          break;
      }
    }
  }
}
