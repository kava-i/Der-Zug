#include "player.h" 
#include "game/config/attributes.h"
#include "tools/calc_enums.h"
#include "tools/func.h"
#include "tools/webcmd.h"
#include "tools/gramma.h"
#include "tools/webcmd.h"
#include <cctype>
#include <memory>
#include <numeric>
#include <string>

#define GREEN Webcmd::set_color(Webcmd::color::GREEN)
#define RED Webcmd::set_color(Webcmd::color::RED)
#define BLUE Webcmd::set_color(Webcmd::color::BLUE)
#define YELLOW Webcmd::set_color(Webcmd::color::YELLOW)
#define WHITE Webcmd::set_color(Webcmd::color::WHITE)
#define WHITEDARK Webcmd::set_color(Webcmd::color::WHITEDARK)
#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

std::shared_ptr<CGramma> CPlayer::_gramma = nullptr;

/**
* Full constructor for player
* @param jAtts json with all attributes
* @param room current room of payer
* @param newAttacks attacks of player
*/
CPlayer::CPlayer(nlohmann::json jAtts, CRoom* room, attacks lAttacks, std::string path,
		const std::map<std::string, ConfigAttribute>& all_attributes) 
    : CPerson(jAtts, nullptr, lAttacks, nullptr, this, std::map<std::string, CDialog*>(), 
				all_attributes) {
  //Set login data and player information
  func::convertToUpper(name_);
  m_sPassword = jAtts["password"];
  m_firstLogin = true; 
	gameover_ = false;
	player_description_ = jAtts.value("player_description", "");

  //Initiazize world
  m_world = new CWorld(this, path);

  m_parser = new CParser(m_world->getConfig());
  
  //Extract minds from config
  for (auto it : m_world->getConfig()["minds"]) {
    SMind mind;
    mind.sID = it["id"];
    mind.color = Webcmd::COLOR_MAPPING.at(it.value("color", "white"));
    mind.level = it["level"].get<int>();
    mind.relevance = it["relevance"].get<double>();
    m_minds[it["id"]] = mind;
  }

  //Attributes
  if (m_world->getConfig().count("attributes")>0) {
    std::vector<std::string> attributes = m_world->getConfig()["attributes"];
    for (auto it : attributes)   
      attributes_[it] = 0;
  }

  // States, f.e. current fight, Dialog-partner
  m_curFight = nullptr;
  m_curDialogPartner = nullptr;

  //Set current room
  m_room = room;
	m_lastRoom = nullptr;

  // Set player's equipment (no equipment at the beginning)
	for (const auto& kind : m_world->item_config()._kinds.at("equipe")) {
  	m_equipment[kind] = nullptr;
	}
  
  //Initialize all rooms as not visited
  for(const auto& it : m_world->getRooms())
    m_vistited[it.second->id()] = false;
  m_room = m_world->getRoom(m_room->id());
  m_vistited[m_room->id()] = true;

  std::cout << "initializing context stack.\n";
  // Initialize context stack with all quests which are active from the beginning.
  for (auto it : m_world->getQuests()) {
    if (it.second->getOnlineFromBeginning() == false) 
      continue;
    // Create context for this quest.
    Context* context = new Context({{"name", it.first}, {"permeable",true}, {"questID", it.first}});
    // Add all quest listeners
    for (auto& listener : it.second->listeners())
      context->AddListener(listener);
    // Add quest-context to context-stack.
    m_contextStack.insert(context, 3, it.first);
  }
  std::cout << "Done.\n";

  cur_music_ = m_world->media("music/background");

  // Add eventhandlers to eventmanager.
  m_contextStack.insert(new Context((std::string)"first"), 9, "first");
  m_contextStack.insert(new Context((std::string)"world"), 2, "world");
  m_contextStack.insert(new Context((std::string)"standard"), 0, "standard");
  updateRoomContext();

  // Add quests
  if (jAtts.count("quests") > 0) {
    for (const auto& it : jAtts["quests"])
      setNewQuest(it);
  }
}

CPlayer::~CPlayer() {
	std::cout << "Deleting player" << std::endl;
  delete m_world;
	std::cout << "- world deleted " << std::endl;
	delete m_parser;
	std::cout << "- parser deleted " << std::endl;
	if (m_curFight) {
		delete m_curFight;
		std::cout << "- fight deleted " << std::endl;
	}
	std::cout << "Deleting player. Done." << std::endl;
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

CRoom* CPlayer::getRoom() { 
  return m_room; 
}

std::string CPlayer::player_description() const {
	return player_description_;
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
  // Setup basic elements
  std::map<std::string, std::string> status = {
		{"room", (std::string)m_room ->id()}, 
		{"last_room", (m_lastRoom) ? m_lastRoom->id() : "---"}, 
		{"inventory", (std::string)m_inventory.getItemList()}
	};

  // Add extra substitutes
  status.insert(subsitutes_.begin(), subsitutes_.end());

  // Get highest mind
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

  // Add attributes
  auto lambda1 = [](int x) { return std::to_string(x); };
  std::map<std::string, std::string> attributes = 
      func::convertToObjectmap(attributes_, lambda1);
  status.insert(attributes.begin(), attributes.end());

  // Add minds
  auto lambda2 = [](SMind mind) { return std::to_string(mind.level); };
  std::map<std::string, std::string> minds = func::convertToObjectmap(m_minds, lambda2);
  status.insert(minds.begin(), minds.end());

	// Add visited rooms
	std::string visited_rooms = "";
	for (const auto& it : m_vistited) {
		if (it.second)
			visited_rooms += ";";
	}
	if (visited_rooms != "")
		visited_rooms.pop_back();
	status["visited_rooms"] = visited_rooms;

  return status;
}

bool CPlayer::gameover() const {
	return gameover_;
}
Webconsole* CPlayer::webconsole() const {
	return _cout;
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

void CPlayer::set_gramma(std::shared_ptr<CGramma> gramma) {
  _gramma = gramma;
}
void CPlayer::set_gameover() {
	gameover_ = true;
}

void CPlayer::SetAttributeAndThrow(std::string attribute_id, int value) {
  setStat(attribute_id, value);
	addPostEvent("attribute_set " + attribute_id);
}

void CPlayer::printText(std::string text) {
	std::string area = "";
	if (text.find(".") != std::string::npos) {
		area = func::split(text, ".")[0];
		text = func::split(text, ".")[1];
	}
  if (m_world->getTexts(area).count(text) > 0) {
    nlohmann::json j_text = m_world->getTexts(area).at(text);
    CText text(j_text, this);
    m_sPrint += text.print();
  }
  else
    std::cout << cRED << "Text not found!" << cCLEAR << std::endl;
}

void CPlayer::appendPrint(std::string sPrint, bool dont_throw_post) {
  throw_staged_events(m_staged_pre_events, "pre");
  if(m_staged_pre_events != "")
    m_sPrint += "\n";
  m_sPrint += sPrint;
	if (!dont_throw_post)
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
	int woozyness = CalculateWoozyness(m_world->attribute_config().woozy_attributes_,
			m_world->attribute_config().woozy_method_);
  sPrint = func::returnSwapedString(sPrint, woozyness);
  if(sSpeaker != "")
    return "<div class='spoken'>" + sSpeaker + " - " + WHITEDARK + sPrint + WHITE + "</div>";
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

  // Create new room context
  Context* context = new Context((std::string)"room");
  // Set room-media 
  context->SetMedia("music", m_room->music());
  context->SetMedia("image", m_room->image());
  
  // Transfer Time events if context exists
  if (m_contextStack.getContext("room") != NULL)
    context->setTimeEvents(m_contextStack.getContext("room")->getTimeEvents());
  // Delete old room-context
  // Update listeners
  for (auto it : m_room->getHandler()) {
		std::cout << "Adding listener from room: " << it->id() << std::endl;
    context->AddListener(it);
	}
  
  // Delete old and insert new room-context into context-stack
  m_contextStack.erase("room");
  m_contextStack.insert(context, 0, "room");
}

void CPlayer::RemoveListenerFromLocation(std::string location, std::string location_id, std::string id) {
  std::cout << "RemoveListenerFromLocation: " << location << ", " << location_id << ", " << id << std::endl;
  if (location == "room") {
    m_world->getRoom(location_id)->RemoveHandler(id);
    updateRoomContext();
  }
  else if (location == "char") {
    m_world->getCharacter(location_id)->RemoveHandler(id);
    updateRoomContext();
  }
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
	m_contextStack.insert(context, 1, "fight");

	m_curFight->InitializeFight();
}

/**
* Delete the current fight and erase fight-context from context-stack
*/
void CPlayer::endFight() {
	if (m_curFight) {
    delete m_curFight;
		m_curFight = nullptr;
  	appendPrint(m_world->GetSTDText("fight_ended") + "\n");
	}
	if (m_contextStack.getContext("fight"))
    m_contextStack.erase("fight");
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
  appendStoryPrint(m_world->GetSTDText("approaching_a") + player->name() 
      + m_world->GetSTDText("approaching_b"));

  if(player->getContexts().nonPermeableContextInList() == true)
    appendStoryPrint(player->name() + m_world->GetSTDText("approaching_busy"));
  else { 
    //Add Chat context for both players
    addChatContext(player->id());
    player->addChatContext(id_);
    
    //Send text by throwing event
    throw_events("Hey " + player->name() + ".", "CPlayer::startChat");
  }
}

/**
* Add chat context.
* @param sPlayer (chat partner (other player))
*/
void CPlayer::addChatContext(std::string sPartner) {
  Context* context = new Context("chat", {{"partner", sPartner}});
  context->AddSimpleListener("h_send", (std::regex)"(.*)", 1, 0);
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
  if (sIdentifier == "back" || sIdentifier == "zurück") {
    changeRoom(m_lastRoom);
    return;
  }

  //Get selected room, checking exits in current room.
  auto lamda1= [](CExit* exit) { return (!exit->hidden()) ? exit->prep() + " " + exit->name() : ""; };
  string room = func::getObjectId(getRoom()->getExtits(), sIdentifier, lamda1);

  if (room != "") {
    changeRoom(getWorld()->getRooms()[room]);
    return;
  }

  //Check all rooms already visited.
  auto lamda2 = [](CRoom* room) { return room->name(); };
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
    appendErrorPrint(m_world->GetSTDText("room_not_found"));
}

/**
* Change room to given room and print entry description. Set last room to current room.
* @param newRoom new room the player changes to
*/
void CPlayer::changeRoom(CRoom* new_room) {
	std::cout << "Changing room to " << new_room->id() << std::endl;
  m_lastRoom = m_room; 
  m_room = new_room;
  std::string entry = new_room->getEntry();
  if(entry != "")
    appendDescPrint(entry);
  appendPrint(m_room->showDescription(m_world->getCharacters()), true);
  m_vistited[m_room->id()] = true;
  updateRoomContext();
	std::cout << "Room updated, throwing post events" << std::endl;
	if (m_staged_post_events == "")	
		m_staged_post_events = "changed_room " + new_room->id();
	else 
		m_staged_post_events += ";changed_room " + new_room->id();
	throw_staged_events(m_staged_post_events, "post");
	std::cout << "Changing room done" << std::endl;
}

/**
* Print all already visited rooms.
*/
void CPlayer::showVisited() {
  std::map<std::string, std::string> visited_rooms;
  for(auto const& it : m_vistited) {
    if(it.second == true)
      visited_rooms[it.first] = m_world->getRoom(it.first)->name();
  }
  if (visited_rooms.size() > 0) {
    appendDescPrint(_gramma->build(func::to_vector(visited_rooms), 
      m_world->GetSTDText("visited_rooms"), "") + m_world->GetSTDText("visited_rooms_info"));
  }
  else {
    appendDescPrint(m_world->GetSTDText("no_visited_rooms"));
  }
}


/**
* Look for shortes way (if exists) to given room. And return way as vector, or empty array.
* @param room players room
* @param roomID id of desired target-room
* @return vector with way to target.
*/
std::vector<std::string> CPlayer::findWay(CRoom* room, std::string roomID) {
  //Check whether current room is target room
  if(room->id() == roomID)
    return {};

  //Set variables
  std::queue<CRoom*> q;
  std::map<std::string, std::string> parents;

  //Set parent of every room to ""
  for(auto it : m_world->getRooms())
    parents[it.second->id()] = "";

  //Normal breadth-first search
  q.push(room);
  parents[room->id()] = room->id();
  while(!q.empty()) {
    CRoom* node = q.front(); 
    q.pop();
    if(node->id() == roomID)
      break;
    for(auto& it : node->getExtits()) {
      //Also check, that target-room is visited and in the same area as current room.
      if(parents[it.first] == "" && m_vistited[it.first] == true 
          && node->getArea() == m_room->getArea()) {
        q.push(m_world->getRoom(it.first));
        parents[it.first] = node->id();
      }
    } 
  }

  //Check whether rooom has been found, if not, return empty array.
  if(parents[roomID] == "") 
    return {};

  //Create path from parents.  
  std::vector<std::string> path = { roomID };
  while(path.back() != room->id())
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
      items_names.push_back((*it).second->name());
      m_room->getItems().erase((*(it++)).second->id());
      continue;
    }
    ++it;
  }
  std::string start = m_world->getConfig()["show"]["add_all_items"]["start"];
  std::string error = m_world->getConfig()["show"]["add_all_items"]["error"];
  appendDescPrint(_gramma->build(items_names, start, error));
}

/**
* Add given item to player's inventory and print message.
* @param item given item/ item to add to inventory.
*/
void CPlayer::addItem(CItem* item) {
  m_inventory.addItem(item);
  appendDescPrint("<b> "+item->name() + "</b>" + m_world->GetSTDText("add_item"));
  m_room->getItems().erase(item->id());
}

/**
* Start a trade. Add a trade-context to player's context stack.
* @param partner Player's trading partner add to context-information.
*/
void CPlayer::startTrade(std::string partner) {
  appendDescPrint(m_world->GetSTDText("started_trade") + partner + "\n");

  //Create trade-context and add to context-stack
  Context* context = new Context((std::string)"trade", {{"partner", partner}});
  context->print(this);
  m_contextStack.insert(context, 1, "trade");
}

/**
* Add all equipped items to player's output.
*/
void CPlayer::printEquiped() {
  std::string nothing_equiped = m_world->GetSTDText("nothing_equipped");
  auto getElem = [nothing_equiped](CItem* item){ 
    std::string str; 
    if(item) 
      str = item->name(); 
    else 
      str = nothing_equiped;
    return str;
  };
  appendPrint(func::table(m_equipment, getElem, "width:50%"));
}

/**
* Equipe given item into given category (sType). If there is already an item equipped in this 
* category, add choice-context to context-stack.
* @param item given item.
* @param sType type of item indicating category.
*/
void CPlayer::equipeItem(CItem* item, string kind_) {
  // If nothing is equipped in this category -> equip.
  if (m_equipment.at(kind_) == nullptr) {
    appendDescPrint(m_world->GetSTDText("equipped_a") + item->name() 
        + m_world->GetSTDText("equipped_b") + kind_ + m_world->GetSTDText("equipped_c"));

    // Check for new attack
    string attack = item->getAttack();
    if (attack != "") {
      m_attacks[attack] = m_world->getAttack(attack);
      appendDescPrint(m_world->GetSTDText("new_attack") + m_attacks[attack]->getName() + "\n");
    }
    // Do updates
		std::string updated;
    Update(item->update(), updated);

    m_equipment[kind_] = item;
		if (item->use_description())
    	appendPrint(item->use_description()->print(true));
		appendPrint(updated);
  }

  // If this item is already equipped -> print error message.
  else if (m_equipment.at(kind_)->id() == item->id())
    appendErrorPrint(kind_ + m_world->GetSTDText("already_equipped"));

  // If another item is equipped in this category -> add choice-context
  else {
    appendErrorPrint(m_world->GetSTDText("already_equipped_change_a" )+ kind_ 
        + m_world->GetSTDText("already_equipped_change_b"));
    // Create Choice-Context
    m_contextStack.insert(new Context((nlohmann::json){{"name", "equipe"}, {"permeable", false},
          {"error", "Wähle ja oder nein\n"}, {"itemID", item->id()},
          {"listeners", {
            {"ja", {"h_choose_equipe"}}, {"nein", {"h_choose_equipe"}},
            {"yes", {"h_choose_equipe"}}, {"no", {"h_choose_equipe"}}
          }}}), 1, "equipe");
  }
}

/**
* Dequipe an item. Erase attack from list of attacks if it depended on equipped item.
* @param sType item type (weapon, clothing etc.)
*/
void CPlayer::dequipeItem(string sType) {
  std::cout << "dequipping item: " << sType << std::endl;
  std::string type_from_item = (m_inventory.getItem(sType)) ? m_inventory.getItem(sType)->kind() : "";
  if (m_equipment.count(type_from_item) > 0) {
    sType = type_from_item;
  }
  if (m_equipment.count(sType) == 0) {
    appendErrorPrint(m_world->GetSTDText("no_dequipe") + "\n");
  }
  else if (m_equipment[sType] == NULL)
    appendErrorPrint(m_world->GetSTDText("no_dequipe") + "\n");
  else {
    appendDescPrint(sType + " " + m_equipment[sType]->name() + m_world->GetSTDText("dequiped"));
    //Erase attack
    if (m_equipment[sType]->getAttack() != "")
      m_attacks.erase(m_equipment[sType]->getAttack());
    // Remove update 
		std::string updated;
    Update(m_equipment[sType]->update().Reverse(), updated);
		appendPrint(updated);

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
void CPlayer::setNewQuest(std::string quest_id) {
  CQuest* quest = m_world->getQuest(quest_id);
	std::string output = quest->setActive(this);
	if (!quest->silent())
  	appendSuccPrint(output);
  std::cout << "setNewQuest: " << quest->getID() << ", active: " << quest->getOnlineFromBeginning() << std::endl;
  // Create new quest-context with all quest-listeners.
  if (quest->getOnlineFromBeginning() == false) {
    Context* context = new Context({{"name", quest_id}, {"permeable",true}, {"questID", quest_id}});
    for (auto& listener : quest->listeners()) {
      std::cout << "Adding listener: " << listener->id() << "|" << listener->handler() << std::endl;
      context->AddListener(listener);
    }
    m_contextStack.insert(context, 3, quest_id);
  }
  if (quest->getSolved() == true) {
		std::string updated;
		Update(quest->updates(), updated);
		appendPrint(updated);

    m_contextStack.erase(quest_id);
	}
}

/**
* Set a quest-step as solved and add received ep (experience points) to players ep.
* @param sQuestID identifier to quest.
* @param sStepID identifier to quest-step.
*/
void CPlayer::questSolved(std::string quest_id, std::string step_id) {
	auto quest = m_world->getQuest(quest_id);

  quest->getSteps()[step_id]->solved(this);
  if (quest->getSolved() == true) {
		std::string updated;
		Update(quest->updates(), updated);
		appendPrint(updated);
    m_contextStack.erase(quest_id);
	}
}


// *** Minds and Level *** //

/**
* Let player know how many learning points player can assign and add choice context.
* @param numPoints experience points player can assign.
*/
void CPlayer::UpdateStats(int num_points, int value) {
  // Create context.
  std::string info_msg = m_world->GetSTDText("update_stats");
  Context* context = new Context((nlohmann::json){{"name", "updateStats"}, {"permeable", false}, 
      {"numPoints", num_points}, {"value", value}, {"error", info_msg}});
  context->AddSimpleListener("h_updateStats", m_world->attribute_config().SkillableNames(true), 0);
  m_contextStack.insert(context, 1, "updateStats");

  //Print attributes and level of each attribute and add choice-context.
	PrintSkillableAttributes(num_points);
}

int CPlayer::GetValueFromAttributeOrMin(std::string name) {
  if (m_minds.count(name) > 0) {
    return m_minds.at(name).level;
  }
  if (attributes_.count(name) > 0) {
    return attributes_.at(name);
  }
  return 1;
}

void CPlayer::PrintSkillableAttributes(int available_points) {
  //Print attributes and level of each attribute and add choice-context.
  m_sPrint += m_world->GetSTDText("points_left_a") + std::to_string(available_points) 
    + m_world->GetSTDText("points_left_b") + "\n";

	int counter = 0;
	const auto& attributes = m_world->attribute_config().attributes_;
	for (const auto& it : m_world->attribute_config().skillable_) {
    m_sPrint += std::to_string(++counter) + ". " + attributes.at(it).name_ 
			+ ": " + std::to_string(getStat(it)) + "\n";
	}
  m_sPrint += m_world->GetSTDText("update_stats") + "\n";
}

/**
* Print minds of player by using table function.
*/
void CPlayer::showMinds() {
  m_sPrint += " --- " + name_ + " --- \n";
	auto lamda = [](SMind mind) { return std::to_string(mind.level);};
  m_sPrint += func::table(m_minds, lamda);
}

/**
* Print player's stats by using table function.
*/
void CPlayer::showStats() {
  m_sPrint += "--- " + name_ + "---\n";
	if (player_description_ != "")
		m_sPrint += "<i>" + player_description_ + "</i>\n";
  
  // Get mapping from config.
	const auto& attribute_conf = m_world->attribute_config().attributes_;
  const auto& mapping = m_world->attribute_config().mapping_;

  std::map<std::string, std::map<std::string, std::string>> stats;
  for (auto it : attributes_) {  
    std::string category = attribute_conf.at(it.first).category_;
    std::string name = attribute_conf.at(it.first).name_;

		// Don't print attribute if category starts with underscore 
		if (category.length() > 0 && category[0] == '_')
			continue;

    if (mapping.count(it.first) > 0) {
      for (const auto& map : mapping.at(it.first)) {
        auto val = map.check_mapping(it.second);
        if (val.has_value()) {
          stats[category][name] = val.value();
          break;
        }
      }
    }
    else {
      stats[category][name] = std::to_string(it.second);
    }
  }
  auto lambda_key = [](const auto& key, const auto&) { return key; };
  auto lambda_val = [](const auto&, const auto& val) { return val; };
  m_sPrint += func::table(stats, lambda_key, lambda_val, "width:20%");
}

void CPlayer::Update(const Updates& updates) {
	std::string updated_print;
	Update(updates, updated_print);
}

void CPlayer::Update(const Updates& updates, std::string& updated_print) {
	const auto& attribute_conf = m_world->attribute_config().attributes_;
	for (const auto& it : updates.updates()) {
    // Since GetValueFromAttributeOrMin returns 1 on failure, nothing will change.
    double value = it.value_ * GetValueFromAttributeOrMin(it.attribute_);
    std::string msg = calc::MOD_TYPE_MSG_MAPPING.at(it.mod_type_);
		// Update minds
		if (m_minds.count(it.id_) > 0) {
			it.ApplyUpdate(m_minds.at(it.id_).level, value);
      updated_print += m_minds.at(it.id_).color + it.id_ + msg + func::dtos(value) + WHITE + "\n";
		}
		else if (attributes_.count(it.id_) > 0) {
      int val_old = attributes_.at(it.id_);
			it.ApplyUpdate(attributes_.at(it.id_), value);
			addPostEvent("attribute_set " + it.id_);
			std::string name = attribute_conf.at(it.id_).name_;
      std::string color = (val_old <= attributes_.at(it.id_)) ? GREEN : RED;
			if (!attribute_conf.at(it.id_).Hidden())
      	updated_print += color + name + msg + func::dtos(value) + WHITE + "\n";
			auto events = attribute_conf.at(it.id_).GetExceedBoundEvents(attributes_.at(it.id_));
			if (events.size() > 0) {
				std::cout << "Update: Adding post-events: " << events << std::endl;
    		addPostEvent(events);
			}
		}
		// Temporary update
		if (it.tmp_) {
			std::string infos = it.ToString(true, true);
			double duration = static_cast<double>(it.secs_)/60;
      getContext("standard")->add_timeEvent("t_setAttribute", "standard", infos, duration);
		}
	}
}

bool CPlayer::CompareUpdates(const Updates& updates) {
	const auto& attribute_conf = m_world->attribute_config().attributes_;
	for (const auto& it : updates.updates()) {
    // Since GetValueFromAttributeOrMin returns 1 on failure, nothing will change.
    double value = it.value_ * GetValueFromAttributeOrMin(it.attribute_);
		// Update minds
		if (m_minds.count(it.id_) > 0) {
      int cur = m_minds.at(it.id_).level;
			it.ApplyUpdate(cur, value); // only a comy is updated
      if (cur <= 0)
        return true;
		}
		else if (attributes_.count(it.id_) > 0) {
      int cur = attributes_.at(it.id_);
			it.ApplyUpdate(cur, value); // only a comy is updated
      if (!attribute_conf.at(it.id_).CheckBounds(cur))
        return true;
		}
  }
  return false;
}

std::string CPlayer::SimpleUpdate(const Updates& updates, std::string& msg, 
		const std::map<std::string, ConfigAttribute>& conf_attributes, bool) {
	std::cout << "SimpleUpdate called from PLAYER" << std::endl;
	return CPerson::SimpleUpdate(updates, msg, conf_attributes, true);
}


// *** Others *** // 

/**
 * Check if player's output contains special commands such as printing player name or else.
 */ 
void CPlayer::checkCommands() {
	while (m_sPrint.find("{") != std::string::npos) {
		size_t pos = m_sPrint.find("{");
		size_t pos2 = m_sPrint.find("}"); 
		std::string cmd = m_sPrint.substr(pos+1, pos2-(pos+1));
		std::string replace = "";
		if (cmd.find("CNAME") != std::string::npos && m_curDialogPartner != nullptr)
			replace = m_curDialogPartner->color() + func::returnToUpper(m_curDialogPartner->name()) + WHITE;
		else if (cmd.find("cname") != std::string::npos && m_curDialogPartner != nullptr) {
			replace = m_curDialogPartner->color() + m_curDialogPartner->name() + WHITEDARK;
		}
		else if (cmd.find("NAME") != std::string::npos)
			replace = color() + func::returnToUpper(name()) + WHITE;
		else if (cmd.find("name") != std::string::npos)
			replace = color() + name() + WHITEDARK;
		m_sPrint = m_sPrint.substr(0, pos) + replace + m_sPrint.substr(pos2+1);
	}

	for(const auto& it : m_minds) {
		size_t pos = 0;
		for(;;) {
			if (m_sPrint.find(func::returnToUpper(it.first), pos) == std::string::npos)
				break;
			pos = m_sPrint.find(func::returnToUpper(it.first), pos);
			m_sPrint.insert(pos, it.second.color);
			m_sPrint.insert(pos + it.first.length()+1, WHITE);
			pos+=it.first.length();
		}
	}

	size_t pos = m_sPrint.rfind("$");
	if (pos != std::string::npos)
			m_sPrint.erase(pos, 1);
}

/**
* Check whether given password and name matches this player.
* @param sName name to compare to player's name.
* @param sPassword password to compare to player's password.
*/
string CPlayer::doLogin(string id, string password) {
  if(id == func::returnToLower(id_) && password== m_sPassword) 
    return id_;
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
    auto lambda = [](CPlayer* player) { return player->name(); };
    std::string sPlayerID = func::getObjectId(m_players, sIdentifier, lambda);

    //Check if player's id could be retrieved.
    if(sPlayerID != "")
        return m_players[sPlayerID];

    //If not, print error message on console and for player and return null-pointer.
    printError("FATAL ERROR!!! Player accessed that does not exist: " + sIdentifier);
    return nullptr;
}

void CPlayer::printError(std::string sError) {
  std::cout << "\033[1;31m"+sError+"\033[0m"<< std::endl;
  appendTechPrint(m_world->GetSTDText("tech_error"));
}

int CPlayer::CalculateWoozyness(std::vector<std::string> attributes, WoozyMethods method) {
	if (method == WoozyMethods::ADD) {
		int i = std::accumulate(attributes.begin(), attributes.end(), 0, 
        [this](int initial, auto att) -> int { 
				  return initial + this->getStat(att, 0);
	      });
    return i;
  }
	return 0;
}

std::string CPlayer::getContextMusic(std::string new_music) {
  std::string music = "";

  // Priorize context-specific music: game-music > player-music > fight > dialog > room
  if (new_music != "")
    music = new_music;
  else if (m_contextStack.getContext("fight") 
      && m_contextStack.getContext("fight")->GetFromMedia("music") != "")
    music = m_contextStack.getContext("fight")->GetFromMedia("music");
  else if (m_contextStack.getContext("dialog")
      && m_contextStack.getContext("dialog")->GetFromMedia("music") != "")
    music = m_contextStack.getContext("dialog")->GetFromMedia("music");
  else if (m_contextStack.getContext("room")
      && m_contextStack.getContext("room")->GetFromMedia("music") != "")
    music = m_contextStack.getContext("room")->GetFromMedia("music");

  // If not, priorize player-specific.
  if (music == "")
    music = music_;
  // Finally game-music.
  if (music == "") 
    music = m_world->media("music/background");

  if (music != "" && music != cur_music_) {
    std::cout << "Updated music to: " << music << std::endl; 
    cur_music_ = music;
    return Webcmd::set_sound(music) + " ";
  }
  else {
    return "";
  }
}

std::string CPlayer::getContextImage(std::string new_image) {
  std::string image = "";
  // Priorize context-specific image.
  if (new_image != "")
    image = new_image;
  if (m_contextStack.getContext("fight"))
    image = m_contextStack.getContext("fight")->GetFromMedia("image");
  else if (m_contextStack.getContext("dialog")
      && m_contextStack.getContext("dialog")->GetFromMedia("image") != "")
    image = m_contextStack.getContext("dialog")->GetFromMedia("image");
  else if (m_contextStack.getContext("room")
      && m_contextStack.getContext("room")->GetFromMedia("image") != "")
    image = m_contextStack.getContext("room")->GetFromMedia("image");

  // If not, priorize player-specific.
  if (image == "")
    image = image_;
  // Finally game-image.
  if (image == "") 
    image = m_world->media("image/background");

  if (image != "" && image != cur_image_) {
    std::cout << "Updated image to: " << image << std::endl; 
    cur_image_ = image;
    return Webcmd::set_image(image) + " ";
  }
  else {
    return "";
  }
}

void CPlayer::updateMedia() {
  std::string music = getContextMusic("");
  if (music != "")
    m_sPrint += music;
  std::string image = getContextImage("");
  if (image != "")
    m_sPrint += image;
}

// *** Eventmanager functions *** //

void CPlayer::throw_events(string sInput, std::string sMessage) {
  updateRoomContext();
  std::cout << cRED << "Events: " << sInput << ", from: " << sMessage << cCLEAR << std::endl;

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
    updateMedia();
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
    updateMedia();
  }
}
