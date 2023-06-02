#include "context.h"
#include "actions/dialog.h"
#include "concepts/quest.h"
#include "eventmanager/listener.h"
#include "eventmanager/sorted_context.h"
#include "fmt/format.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include "objects/item.h"
#include "objects/person.h"
#include "objects/player.h"
#include "game/game.h"
#include "tools/calc_enums.h"
#include "tools/func.h"
#include "tools/webcmd.h"
#include <cstddef>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
#define cRED "\033[1;31m"
#define cGreen "\033[1;32m"
#define cBlue "\033[1;34m"
#define cCLEAR "\033[0m"


// ***** ***** CONSTRUCTORS ***** ***** //

std::map<std::string, void (Context::*)(std::string&, CPlayer* p)> Context::listeners_ = {};
std::map<std::string, nlohmann::json> Context::m_templates = {};

Context::Context(nlohmann::json json) {
  m_jAttributes = json;
  m_sName = json.value("name", "context");
  m_permeable = json.value("permeable", false);
  m_curPermeable = m_permeable;
  m_block = false;
  m_sHelp = json.value("help", "");
  m_sError = json.value("error", "");

  // listeners
  for (const auto &it : json.value("listeners", std::map<string, vector<string>>())) {
    std::string event_type = it.first;
    for(size_t i=0; i<it.second.size(); i++)
      AddSimpleListener(it.second[i], event_type, 0-i);
  }
  AddSimpleListener("h_help", "help", 0);
  m_error = &Context::error;
}
 
Context::Context(std::string sTemplate) : Context(getTemplate(sTemplate)) {}

Context::Context(std::string sTemplate, nlohmann::json json) : Context(getTemplate(sTemplate)) {
  m_jAttributes = json;
  m_sName = json.value("name", m_sName);
  m_permeable = json.value("permeable", m_permeable);
  m_curPermeable = m_permeable;
  m_sHelp = json.value("help", m_sHelp);
  m_sError = json.value("error", m_sError);

  // listeners
  for (const auto &it : json.value("listeners", std::map<string, vector<string>>())) {
    std::string event_type = it.first;
    for(size_t i=0; i<it.second.size(); i++)
      AddSimpleListener(it.second[i], event_type, 0-i);
  }
  m_error = &Context::error;
}

// ***** ***** GETTER ***** ***** //
std::string Context::getName() {
  return m_sName;
}

bool Context::getPermeable() {
  return m_permeable;
}

nlohmann::json Context::getTemplate(std::string sTemplate) {
  if(m_templates.count(sTemplate) == 0) {
    std::cout << "Template requested that does not exist: " << sTemplate << std::endl;
    return nlohmann::json::object();
  }
  return m_templates[sTemplate];
} 

nlohmann::json& Context::getAttributes() {
  return m_jAttributes;
}

std::string Context::GetFromMedia(std::string type) {
  if (media_.count(type) > 0)
    return media_.at(type);
  return "";
}


// ***** ***** SETTER ***** ***** //
void Context::setName(std::string sName) {
  m_sName = sName;
}

void Context::setGame(CGame* game) {
  m_game = game;
}

void Context::setErrorFunction(void(Context::*func)(CPlayer* p)) {
  m_error = func;
}

void Context::setCurPermeable(bool permeable) {
  m_curPermeable = permeable;
}

void Context::SetMedia(std::string type, std::string filename) {
  media_[type] = filename;
}

// ***** ***** INITIALIZERS ***** ***** //
void Context::initializeHanlders() {
  // ***** GENERAL STUFF ***** //
  listeners_["h_help"] = &Context::h_help;

  // ***** GAME CONTEXT ***** //
  listeners_["h_reloadGame"] = &Context::h_reloadGame;
  listeners_["h_reloadPlayer"] = &Context::h_reloadPlayer;
  listeners_["h_reloadWorlds"] = &Context::h_reloadWorlds;
  listeners_["h_reloadWorld"] = &Context::h_reloadWorld;
  listeners_["h_updateListeners"] = &Context::h_updatePlayers;

  // ***** WORLD CONTEXT ***** //
  listeners_["h_printText"] = &Context::h_printText;
  listeners_["h_finishCharacter"] = &Context::h_finishCharacter;
  listeners_["h_killCharacter"] = &Context::h_killCharacter;
  listeners_["h_deleteCharacter"] = &Context::h_deleteCharacter;
  listeners_["h_addItem"] = &Context::h_addItem;
  listeners_["h_removeItem"] = &Context::h_removeItem;
  listeners_["h_newFight"] = &Context::h_newFight;
  listeners_["h_endFight"] = &Context::h_endFight;
  listeners_["h_endDialog"] = &Context::h_endDialog;
  listeners_["h_gameover"] = &Context::h_gameover;
  listeners_["h_addQuest"] = &Context::h_addQuest;
  listeners_["h_showPersonInfo"] = &Context::h_showPersonInfo;
  listeners_["h_showItemInfo"] = &Context::h_showItemInfo;
  listeners_["h_changeName"] = &Context::h_changeName;
  listeners_["h_addExit"] = &Context::h_addExit;
  listeners_["h_setNewAttribute"] = &Context::h_setNewAttribute;
  listeners_["h_addTimeEvent"] = &Context::h_addTimeEvent;
  listeners_["h_setNewQuest"] = &Context::h_setNewQuest;
  listeners_["h_changeDialog"] = &Context::h_changeDialog;
  listeners_["h_changeRoomSilent"] = &Context::h_changeRoomSilent;
  listeners_["h_changeRoom"] = &Context::h_changeRoom;
  listeners_["h_startDialogDirect"] = &Context::h_startDialogDirectB;
  listeners_["h_changeSound"] = &Context::h_changeSound;
  listeners_["h_changeImage"] = &Context::h_changeImage;

  // Room modifiers
  listeners_["h_addCharToRoom"] = &Context::h_addCharToRoom;
  listeners_["h_addDetailToRoom"] = &Context::h_addDetailToRoom;
  listeners_["h_addItemToRoom"] = &Context::h_addItemToRoom;

  listeners_["h_moveCharToRoom"] = &Context::h_moveCharToRoom;
  listeners_["h_moveDetailToRoom"] = &Context::h_moveDetailToRoom;
  listeners_["h_moveItemToRoom"] = &Context::h_moveItemToRoom;

  listeners_["h_removeCharFromRoom"] = &Context::h_removeCharFromRoom;
  listeners_["h_removeDetailFromRoom"] = &Context::h_removeDetailFromRoom;
  listeners_["h_removeItemFromRoom"] = &Context::h_removeItemFromRoom;
  listeners_["h_removeHandler"] = &Context::h_removeHandler;

  listeners_["h_setAttribute"] = &Context::h_setAttribute; 
  listeners_["h_setAttributeBound"] = &Context::h_setAttributeBound; 
  listeners_["h_setMind"] = &Context::h_setMind; 


  // ***** STANDARD CONTEXT ***** //
  listeners_["h_show"] = &Context::h_show;
  listeners_["h_look"] = &Context::h_look;
  listeners_["h_search"] = &Context::h_search;
  listeners_["h_take"] = &Context::h_take;
  listeners_["h_goTo"] = &Context::h_goTo;
  listeners_["h_use"]  = &Context::h_use;
  listeners_["h_consume"] = &Context::h_consume;
  listeners_["h_read"] = &Context::h_read;
  listeners_["h_equipe"]  = &Context::h_equipe;
  listeners_["h_dequipe"] = &Context::h_dequipe;
  listeners_["h_examine"] = &Context::h_examine;
  listeners_["h_startDialog"] = &Context::h_startDialog;
  listeners_["h_ignore"] = &Context::h_ignore;
  listeners_["h_try"] = &Context::h_try;

  //m_handlers["h_firstZombieAttack"] = &Context::h_firstZombieAttack;
  listeners_["h_moveToHospital"] = &Context::h_moveToHospital;
  listeners_["h_exitTrainstation"] = &Context::h_exitTrainstation;
  listeners_["h_thieve"] = &Context::h_thieve;
  listeners_["h_attack"] = &Context::h_attack;

  listeners_["h_test"] = &Context::h_test;

  // *** FIGHT CONTEXT *** //
  listeners_["h_fight"] = &Context::h_fight;

  // *** DIALOG CONTEXT *** //
  listeners_["h_call"] = &Context::h_call;

  // *** TRADECONTEXT *** //
  listeners_["h_sell"] = &Context::h_sell;
  listeners_["h_buy"] = &Context::h_buy;
  listeners_["h_exit"] = &Context::h_exit;

  // *** CHATCONTEXT *** //
  listeners_["h_send"] = &Context::h_send;
  listeners_["h_end"] = &Context::h_end;

  // *** READCONTEXT *** //
  listeners_["h_stop"] = &Context::h_stop;
  listeners_["h_next"] = &Context::h_next;
  listeners_["h_prev"] = &Context::h_prev;
  listeners_["h_mark"] = &Context::h_mark;
  listeners_["h_underline"] = &Context::h_underline;

  listeners_["h_next"] = &Context::h_next;


  // *** QUESTS *** //
  listeners_["h_react"] = &Context::h_react;
  listeners_["reden"] = &Context::h_reden;
  listeners_["geldauftreiben"] = &Context::h_geldauftreiben;

  nlohmann::json availible_handlers = nlohmann::json::array();
  for (const auto& it : listeners_) 
    availible_handlers.push_back(it.first);
  std::cout << "Writing availible_handlers handlers to disc..." << std::endl;
  func::WriteJsonToDics("../gui/webserver/handlers.json", availible_handlers); 


  // *** PROGRAMMER *** //

  // *** OTHER CONTEXT *** //
  listeners_["h_select"] = &Context::h_select;
  listeners_["h_choose_equipe"] = &Context::h_choose_equipe;
  listeners_["h_updateStats"] = &Context::h_updateStats;
  listeners_["h_levelUp"] = &Context::h_levelUp;

  // --- *** TIMEEVENTS *** --- //
  listeners_["t_throwEvent"] = &Context::t_throwEvent;
  listeners_["t_setAttribute"] = &Context::t_setAttribute;
}

void Context::initializeTemplates() {
    m_templates["game"] = {
                    {"name", "game"}, {"permeable", true}, 
                    {"listeners", {
                        {"reload_game",{"h_reloadGame"}},
                        {"reload_player",{"h_reloadPlayer"}},
                        {"reload_world", {"h_reloadWorld"}},
                        {"reload_worlds",{"h_reloadWorlds"}},
                        {"update_players",{"h_updatePlayers"}}}}
                    };

    m_templates["first"] = {
                    {"name", "first"}, {"permeable", true},
                    {"listeners", {
                        {"show",{"h_show"}}, 
                        {"examine",{"h_examine"}} }}
                    };

    m_templates["world"] = {
                    {"name", "world"}, {"permeable", true},
                    {"listeners", {
                        {"finishCharacter", {"h_finishCharacter"}},
                        {"killCharacter", {"h_killCharacter"}},
                        {"deleteCharacter", {"h_deleteCharacter"}},
                        {"addItem", {"h_addItem"}},
                        {"removeItem", {"h_removeItem"}},
                        {"fight", {"h_newFight"}},
                        {"endFight",{"h_endFight"}},
                        {"endDialog",{"h_endDialog"}},
                        {"gameover",{"h_gameover"}},
                        {"addQuest",{"h_addQuest"}},
                        {"showperson",{"h_showPersonInfo"}},
                        {"showitem",{"h_showItemInfo"}},
                        {"changeName",{"h_changeName"}},
                        {"addExit",{"h_addExit"}},
                        {"setAttribute", {"h_setAttribute"}},
                        {"setAttributeBound", {"h_setAttributeBound"}},
                        {"updateStats", {"h_updateStats"}},
                        {"levelUp", {"h_levelUp"}},
                        {"setMind", {"h_setMind"}},
                        {"setNewAttribute", {"h_setNewAttribute"}}, 
                        {"addTimeEvent", {"h_addTimeEvent"}},
                        {"setNewQuest", {"h_setNewQuest"}},
                        {"changeDialog", {"h_changeDialog"}},
                        {"changeRoomSilent", {"h_changeRoomSilent"}},
                        {"changeRoom", {"h_changeRoom"}},
                        {"startDialogDirekt", {"h_startDialogDirect"}},
                        {"changeImage", {"h_changeImage"}},
                        {"changeSound", {"h_changeSound"}},
                        {"addCharToRoom",   {"h_addCharToRoom"}},
                        {"addDetailToRoom", {"h_addDetailToRoom"}},
                        {"addItemToRoom",   {"h_addItemToRoom"}},
                        {"moveCharToRoom",   {"h_moveCharToRoom"}},
                        {"moveDetailToRoom", {"h_moveDetailToRoom"}},
                        {"moveItemToRoom",   {"h_moveItemToRoom"}},
                        {"removeCharFromRoom",   {"h_removeCharFromRoom"}},
                        {"removeDetailFromRoom", {"h_removeDetailFromRoom"}},
                        {"removeItemFromRoom",   {"h_removeItemFromRoom"}},
                        {"attribute_set",{"h_ignore"}}, 
                        {"removeHandler", {"h_removeHandler"}} }}
                    };

    m_templates["standard"] = {
                    {"name", "standard"}, {"permeable",false}, 
                    {"help","standard.txt"},   
                    {"listeners",{
                        {"go", {"h_goTo"}},
                        {"look",{"h_look"}}, 
                        {"search",{"h_search"}}, // just as look but tries without specifying location
                        {"talk",{"h_startDialog"}}, 
                        {"pick",{"h_take"}}, 
                        {"use",{"h_use"}}, 
                        {"consume",{"h_consume"}}, 
                        {"read",{"h_read"}}, 
                        {"equipe",{"h_equipe"}}, 
                        {"dequipe",{"h_dequipe"}}, 
                        {"show",{"h_ignore"}}, 
                        {"examine",{"h_ignore"}},
                        {"changed_room",{"h_ignore"}},
                        {"attribute_set",{"h_ignore"}},
                        {"try", {"h_try"}}}},
                    };

    m_templates["fight"] = {
                    {"name","fight"}, {"permeable",false}, {"help","fight.txt"}, 
                    {"listeners",{
                        {"use_against",{"h_fight"}}, 
                        {"attribute_set",{"h_ignore"}}, 
												{"h_fight", {"h_fight"}}}}
                    };

    m_templates["dialog"] = {
				{"name","dialog"}, 
				{"permeable",false}, 
				{"help","dialog.txt"},
				{"error","Das kann ich beim besten Willen nicht sagen!"},
				{"listeners",{ {"changed_room", {"h_ignore"}} }}
		};

    m_templates["trade"] = {
                    {"name","trade"}, {"permeable",false}, {"help","trade.txt"}, 
                    {"listeners",{
                        {"kaufe", {"h_buy"}},
                        {"verkaufe",{"h_sell"}},
                        {"back",{"h_exit"}},
                        {"zurück",{"h_exit"}}}}
                    };
    m_templates["chat"] = {
                    {"name", "chat"}, {"permeable", false}, {"help","chat.txt"},
                    {"listeners",{
                        {"[end]",{"h_end"}}}}
                    };

    m_templates["read"] = {
                    {"name","read"}, {"permeable",false}, {"help","read.txt"},
                    {"listeners",{
                        {"next", {"h_next"}},
                        {"previous", {"h_prev"}},
                        {"mark", {"h_mark"}},
                        {"underline", {"h_underline"}},
                        {"end",{"h_stop"}}}}
                    };

    m_templates["room"] = {{"name", "room"}, {"permeable", true,}, {"infos", {}}};

		std::vector<std::string> availible_commands = {"changed_room", "printText", "attribute_set", "opponentDied"};
    for (const auto& temp : m_templates) {
      if (temp.second.contains("listeners")) {
        for (const auto& listener : temp.second["listeners"].get<std::map<std::string, nlohmann::json>>())
            availible_commands.push_back(listener.first);
      }
    }
    func::WriteJsonToDics("../gui/webserver/commands.json", availible_commands);
}

// ***** ***** FUNCTIONS ***** ****** // 


// *** add and delete time events *** //
void Context::add_timeEvent(std::string id, std::string scope, 
    std::string info, double duration, int priority) {
  m_timeevents.insert(new CTimeEvent(id, scope, info, duration), priority, id);
}

bool Context::timeevent_exists(std::string type) {
  if (m_timeevents.getContext(type) != NULL)
    return true;    
  return false;
}

void Context::deleteTimeEvent(std::string name) {
  m_timeevents.erase(name);
}

// *** add and delete listeners *** //

void Context::AddSimpleListener(std::string handler, std::string event_type, int priority) {
  // Cannot be deleted, thus, no id, location, location_id and self_delete=false
  // Als permeable always set to -1 -> dont overwrite context/ handler-permeability
  AddListener(new CListener("", handler, "", "", "", "", -1, false, priority, event_type));
}

void Context::AddSimpleListener(std::string handler, std::regex event_type, int pos, int priority) {
  // Cannot be deleted, thus, no id, location, location_id and self_delete=false
  // Als permeable always set to -1 -> dont overwrite context/ handler-permeability
  AddListener(new CListener("", handler, "", "", "", "", -1, false, priority, event_type, pos));
}

void Context::AddSimpleListener(std::string handler, std::vector<std::string> event_type, int priority) {
  // Cannot be deleted, thus, no id, location, location_id and self_delete=false
  // Als permeable always set to -1 -> dont overwrite context/ handler-permeability
	std::cout << "AddSimpleListener: event_type=" << func::join(event_type, ", ") << std::endl;
  AddListener(new CListener("", handler, "", "", "", "", -1, false, priority, event_type));
}

void Context::AddSimpleListener(std::string handler, std::map<std::string, std::string> event_type, 
    int priority) {
  // Cannot be deleted, thus, no id, location, location_id and self_delete=false
  // Als permeable always set to -1 -> dont overwrite context/ handler-permeability
  AddListener(new CListener("", handler, "", "", "", "", -1, false, priority, event_type));
}

void Context::AddListener(CListener* listener) {
  m_eventmanager.insert(listener, listener->priority(), listener->handler());
  m_jAttributes["infos"][listener->id()] = listener->new_identifier();
}

// *** Throw events *** //
bool Context::throw_event(event e, CPlayer* p) {
  std::cout << cBlue << m_sName << cCLEAR << " throwing: " << e.first << "|" << e.second << std::endl;
  m_curPermeable = m_permeable;
  m_block = false;
  bool called = false;
  m_curEvent = e;
  
  std::deque<CListener*> sortedEventmanager = m_eventmanager.getSortedCtxList();
  for (size_t i=0; i<sortedEventmanager.size() && m_block == false; i++) {
    event cur_event = e;
    if (sortedEventmanager[i]->check_match(cur_event) == true) {
      if (listeners_.count(sortedEventmanager[i]->handler()) > 0) {
        // Call event
        std::cout << cGreen << " ... " << cBlue << m_sName << ": " << cGreen 
          << "event triggered: " << sortedEventmanager[i]->handler()
          << "(" << cur_event.second << ")" << cCLEAR << std::endl;
        (this->*listeners_[sortedEventmanager[i]->handler()])(cur_event.second, p);
        std::cout << cGreen << "Event call done." << cCLEAR << std::endl;
        // If permeable differs from -1, set permeability of context to given permeability.
        if (sortedEventmanager[i]->permeable() != -1) {
          m_curPermeable = sortedEventmanager[i]->permeable();
          std::cout << "permeable set to " << m_curPermeable << std::endl;
        }
        // If self-delete is set, remove listener (only works for object-listeners)
        if (sortedEventmanager[i]->self_delete() == true)
          p->RemoveListenerFromLocation(sortedEventmanager[i]->location(), 
              sortedEventmanager[i]->location_id(), sortedEventmanager[i]->id());
      }
      else {
        p->printError("ERROR, given handler not found: " + sortedEventmanager[i]->handler() + "\n");
      }
      called = true;
    }
  }   

  if (called == false && !func::inArray({"changed_room", "attribute_set"}, e.first))
    error(p);
      
  return m_curPermeable;
}

void Context::throw_timeEvents(CPlayer* p) {
  std::cout << "throw_timeEvents" << std::endl;
  //Check if player is currently occupied.
  if (p->getContexts().nonPermeableContextInList() == true)
    return;

  //Get current time.
  auto end = std::chrono::system_clock::now();

  //Check if a time event is ready to throw, then throw.
  std::deque<CTimeEvent*> sortedEvents = m_timeevents.getSortedCtxList();
  for (size_t i=0; i<sortedEvents.size(); i++) {
    std::chrono::duration<double> diff = end - sortedEvents[i]->getStart();
    std::cout << "throw_timeEvents, git diff: " << diff.count() << std::endl;
    if (diff.count() >= sortedEvents[i]->getDuration()) {
      std::cout << "throwing timeEvent " << std::endl;
      std::cout << "info: " << sortedEvents[i]->getInfo() << std::endl;
      std::cout << "id: " << sortedEvents[i]->getID() << std::endl;
      std::string str = sortedEvents[i]->getInfo();
      (this->*listeners_[sortedEvents[i]->getID()])(str, p);
      std::cout << "BACKPASS: " << str << std::endl;
      if (str == "delete")
        m_timeevents.eraseByPointer(sortedEvents[i]);
    }
  }
}

// ***** ERROR FUNCTIONS ****** ***** //

void Context::error(CPlayer* p) {
  if (m_permeable == false && m_sError != "")
    p->appendErrorPrint(m_sError + " " + m_curEvent.first);
  else if (m_permeable == false) {
    p->appendTechPrint("Falsche Eingabe, gebe \"help\" ein, falls du nicht "
        "weiter weißt. (" + m_sName + ")");
  }
}

void Context::error_delete(CPlayer* p) {
  std::cout << "Error from context " << m_sName << " called. Context will be "
    << "deleted.\n";
  p->getContexts().erase(m_sName); 
}


// ***** ***** ***** HANDLER ***** ***** ***** //

void Context::h_help(std::string &sIdentifier, CPlayer* p) {
  if(m_sHelp != "") {
    std::ifstream read("factory/help/"+m_sHelp);
    std::string str((std::istreambuf_iterator<char>(read)),
                std::istreambuf_iterator<char>());
    p->appendPrint("\n<b>Help: </b>\n" + str);
  }
  else if(m_permeable == false)
    p->appendTechPrint("No help for this context, sorry.\n");
}

// ***** ***** GAME CONTEXT ***** ***** //

void Context::h_reloadGame(std::string&, CPlayer* p) {
  p->appendPrint("reloading game... \n");
  m_curPermeable = false;
}

void Context::h_reloadPlayer(std::string& sPlayer, CPlayer*p) {
	p->appendPrint("reloading Player: " + sPlayer + "... \n");
	std::cout << "CONTEXT: reloading Player: " + sPlayer + "..." << std::endl;
	if (m_game->reloadPlayer(sPlayer) == false) {
		p->appendPrint("Player does not exist... reloading world failed.\n");
		std::cout << "CONTEXT: Player does not exist... reloading world failed." << std::endl;
	}
	else {
		p->appendPrint("Done.\n");
		std::cout << "CONTEXT: Done." << std::endl;
	}
	m_curPermeable = false;
}

void Context::h_reloadWorlds(std::string&, CPlayer*p)
{
    p->appendPrint("reloading all worlds... \n");
    if(m_game->reloadWorld() == true)
        p->appendPrint("Reloading all worlds failed.\n");
    else
        p->appendPrint("Done.\n");
    m_curPermeable = false;
}

void Context::h_reloadWorld(std::string& sPlayer, CPlayer*p)
{
    p->appendPrint("reloading world of: " + sPlayer + "... \n");
    if(m_game->reloadWorld(sPlayer) == false)
        p->appendPrint("Player does not exist... reloading world failed.\n");
    else
        p->appendPrint("Done.\n"); 
    m_curPermeable = false;
}

void Context::h_updatePlayers(std::string&, CPlayer*p)
{
    p->appendPrint("updating players... \n");
    m_game->playerFactory(true); 
    p->appendPrint("done.\n");
    m_curPermeable = false;
}


// ***** ***** WORLD CONTEXT ***** ***** //

void Context::h_printText(std::string& sIdentifier, CPlayer* p) {
	p->printText(sIdentifier);
}

void Context::h_finishCharacter(std::string& sIdentifier, CPlayer* p) {
  std::cout << "h_finishCharacter: " << sIdentifier << std::endl;

  //Get character
  CPerson* person = p->getWorld()->getCharacter(sIdentifier);

	std::string updates;
	p->Update(person->updates(), updates);
	p->appendPrint(updates);

  if(person->will_faint() == false) {
    std::cout << "kill character\n";
    p->throw_events("killCharacter " + sIdentifier, "h_finishCharacter");
  }
  else {
    std::cout << "Character fainted.\n";
    if(person->getDialog("faint") != nullptr)
      person->setDialog("faint");
    else
      person->setDialog(p->getWorld()->getDialog("faintDialog"));
		person->setFainted(true);
  } 
  m_curPermeable=false;
}

void Context::h_killCharacter(std::string& sIdentifier, CPlayer* p) {
  //Get character
  CPerson* person = p->getWorld()->getCharacter(sIdentifier);
  //Create json for details
  nlohmann::json jDetail = { {"id", person->id()},  {"name", person->name()}, {"look", ""}};

  //Parse details-description
  if(person->getDeadDescription().size() == 0) {
		nlohmann::json j = { {"speaker", "story"}, {"text", p->getWorld()->GetSTDText("deafult_dead_desc")} };
		jDetail["description"].push_back(j);
  }
  else
    jDetail["description"] = person->getDeadDescription();

  // Create new detail and add to room and map of details
  CDetail* detail = new CDetail(jDetail, p, person->getInventory().mapItems());
  p->getRoom()->getDetails()[person->id()] = detail;
  h_deleteCharacter(sIdentifier, p);
}

void Context::h_deleteCharacter(std::string& sIdentifier, CPlayer* p) {
  if (p->getWorld()->getCharacters().count(sIdentifier) > 0) {
    p->getRoom()->getCharacters().erase(sIdentifier);
    delete p->getWorld()->getCharacter(sIdentifier);
    p->getWorld()->getCharacters().erase(sIdentifier); 
  }
  else {
    std::cout << cRED << "(h_deleteCharacter): Character not found: " 
      << sIdentifier << cCLEAR << std::endl;
  }
  m_curPermeable=false;
}

void Context::h_addItem(std::string& sIdentifier, CPlayer* p) {
  p->addItem(p->getWorld()->getItem(sIdentifier, p));
  m_curPermeable=false;
}

void Context::h_removeItem(std::string& sIdentifier, CPlayer* p) {
  p->getInventory().removeItemByID(sIdentifier);
  m_curPermeable=false;
}

void Context::h_endFight(std::string&, CPlayer* p) {
  p->endFight();
  m_curPermeable=false;
}

void Context::h_endDialog(std::string&, CPlayer* p) {
  p->getContexts().erase("dialog");
  m_curPermeable=false;
}

void Context::h_newFight(std::string& sIdentifier, CPlayer* p) {
  std::cout << "h_newFight: " << sIdentifier << std::endl;
	std::vector<CPerson*> opponents;

	for (const auto& opponent_name : func::split(sIdentifier, ",")) {
		const auto [name, pos] = GetNthObject(opponent_name); // make sure to also get f.e. zombie-2
		auto lambda = [](CPerson* person) {return person->name(); };
		std::string str = func::getObjectId(p->getRoom()->getCharacters(), name, lambda, pos);
		auto character = p->getWorld()->getCharacter(str);
		if (character && !character->fainted())
			opponents.push_back(character);
		else if (character) {
    	p->startDialog(character->id());  
			return;
		}
	}
	if (opponents.size() > 0)
		p->setFight(new CFight(p, {opponents}));	
	m_curPermeable=false;
}

void Context::h_gameover(std::string&, CPlayer* p) {
  p->appendPrint("\nGame ended.\n\n Press Enter to continue.\n");
	p->set_gameover();
  m_curPermeable=false;
}

void Context::h_addQuest(std::string& sIdentifier, CPlayer* p) {
    p->setNewQuest(sIdentifier);
    m_curPermeable=false;
}

void Context::h_showPersonInfo(std::string& sIdentifier, CPlayer* p) {
  auto lambda = [](CPerson* person) { return person->name();};
  std::string character = func::getObjectId(p->getRoom()->getCharacters(), sIdentifier, lambda);
  if (character != "")
    p->appendPrint(p->getWorld()->getCharacter(character)->getAllInformation());
  else
    p->appendPrint("character not found.\n");
  m_curPermeable=false;
}

void Context::h_showItemInfo(std::string& sIdentifier, CPlayer* p) {
  auto lambda = [](CItem* item) { return item->name(); };
  std::string item = func::getObjectId(p->getRoom()->getItems(), sIdentifier, lambda);
  if (item != "")
    p->appendPrint(p->getRoom()->getItems()[item]->getAllInfos());
  else
    p->appendPrint("Item not found.\n");
  m_curPermeable=false;
}

void Context::h_changeName(std::string& sIdentifier, CPlayer* p) {
  std::vector<std::string> v_atts = func::split(sIdentifier, "|");
  if (v_atts[0] == "character") {
    if (p->getWorld()->getCharacter(v_atts[1]) != NULL) 
      p->getWorld()->getCharacter(v_atts[1])->set_name(v_atts[2]);
    else {
      std::cout << cRED << "Character not found: " << v_atts[1] << cCLEAR << std::endl;
    }
  }
  m_curPermeable=false;
}

void Context::h_addExit(std::string& sIdentifier, CPlayer* p) {
  std::cout << sIdentifier << std::endl;
  std::string room = func::split(sIdentifier, "|")[0];
  std::string linked_room = func::split(sIdentifier, "|")[1];
  if (p->getWorld()->getRoom(room) && p->getWorld()->getRoom(room)->getExtits().count(linked_room) > 0)
    p->getWorld()->getRoom(room)->getExtits().at(linked_room)->set_hidden(false);
  m_curPermeable=false;
}

void Context::h_setAttribute(std::string& sIdentifier, CPlayer* p) {
  //Get vector with [0]=attribute to modify, [1]=operand, [2]=value
  std::cout << "h_setAttribute: " << sIdentifier << std::endl;
  std::vector<std::string> atts = func::split(sIdentifier, "|");

  //Check if sIdentifier contains the fitting values
  if(atts.size() < 3 || std::isdigit(atts[2][0]) == false || p->getStat(atts[0]) == 999) {
    std::cout << "Something went worng! Player Attribute could not be changed.\n";
    return;
  }

	if (calc::MOD_TYPE_STRING_MAPPING.count(atts[1]) == 0) {
    std::cout << "Something went worng! Player Attribute could not be changed: " 
			<< "unkown mod-type: " << atts[1] << ".\n";
    return;
	}

	// Get mod_type and update attribute
	calc::ModType mod_type = calc::MOD_TYPE_STRING_MAPPING.at(atts[1]);
	std::string attribute = atts[0];
  int value = stoi(atts[2]);
	int cur = p->getStat(atts[0]);
	calc::MOD_TYPE_FUNCTION_MAPPING.at(mod_type)(cur, value);
	p->SetAttributeAndThrow(attribute, cur);

	// Add events caused by changing attribute
	auto events = p->getWorld()->attribute_config().attributes_.at(attribute).GetExceedBoundEvents(p->getStat(attribute));
	if (events.size() > 0) {
		p->addPostEvent(events);
	}


	// Get msg (set to, increased, decreased) and color and print msgs
  std::string msg = calc::MOD_TYPE_MSG_MAPPING.at(mod_type);
  Webcmd::color color = Webcmd::color::WHITE;
  if (atts.size() > 3 && atts[3] == "green")
    color = Webcmd::color::GREEN;
  else if (atts.size() > 3 && atts[3] == "red")
    color = Webcmd::color::RED;
	
  if (atts.size() > 3 && !p->getWorld()->attribute_config().attributes_.at(attribute).Hidden())
    p->appendPrint(Webcmd::set_color(color) + atts[0] + msg 
        + std::to_string(value ) + Webcmd::set_color(Webcmd::color::WHITE) + "\n");

  m_curPermeable = false;
}

void Context::h_setAttributeBound(std::string& sIdentifier, CPlayer* p) {
	auto parts = func::split(sIdentifier, "|");
	if (parts.size() != 4 || !isdigit(parts[2][0])) {
		std::cout << cRED << "h_setAttributeBound: invalid args, use: \"[attribute-id]|[(int)new-bound|which-bound\"" 
			<< cCLEAR << std::endl;
	}
	else {
		p->getWorld()->attribute_config_NON_CONST().UpdateBound(parts[0], parts[1], std::stoi(parts[2]), parts[3]=="lower");
	}
  m_curPermeable = false;
}

void Context::h_setMind(std::string& sIdentifier, CPlayer* p) {
  std::cout << "Calling h_setMind with sIdentifier: " << sIdentifier << std::endl;
  //Get vector with [0]=attribute to modify, [1]=operand, [2]=value
  std::vector<std::string> atts = func::split(sIdentifier, "|");

  //Check if sIdentifier contains the fitting values
  if(atts.size() < 3 || std::isdigit(atts[2][0]) == false || p->getMinds().count(atts[0]) == 0) {
    std::cout << "Something went worng! Player mind could not be changed." << std::endl;
    return;
  }

  std::string mind = atts[0];
  int value = stoi(atts[2]);
  std::string msg = "";
  
  //Modify attribute according to operand.
  if(atts[1] == "=") {
    p->getMinds().at(mind).level = value;
    msg = " set to ";
  }
  else if(atts[1] == "+") {
    p->getMinds().at(mind).level += value;
    msg = " increased by ";
  }
  else if(atts[1] == "-") {
    p->getMinds().at(mind).level -= value;
    msg = " decreased by ";
  }
  else
    std::cout << "Wrong operand for setting mind." << "\n";

  Webcmd::color color = Webcmd::color::WHITE;
  if (atts.size() > 3 && atts[3] == "green")
    color = Webcmd::color::GREEN;
  else if (atts.size() > 3 && atts[3] == "red")
    color = Webcmd::color::RED;
  if (atts.size() > 3)
    p->appendPrint(Webcmd::set_color(color) + mind + msg 
        + std::to_string(value )+ Webcmd::set_color(Webcmd::color::WHITE) + "\n");
  m_curPermeable = false;
}


void Context::h_setNewAttribute(std::string& sIdentifier, CPlayer* p) {
  std::vector<std::string> atts = func::split(sIdentifier, "|");
  
  //Check if sIdentifier contains the fitting values
  if(atts.size() != 2 || std::isdigit(atts[1][0]) == false)
    std::cout << "Something went worng! Player Attribute could not be changed.\n";
  else
    p->getStats()[atts[0]] = stoi(atts[1]);
  m_curPermeable=false;
}

void Context::h_addTimeEvent(std::string& sIdentifier, CPlayer* p) {
  std::vector<std::string> atts = func::split(sIdentifier, ",");

  //Check if sIdentifier contains the fitting values
  if(atts.size()!=4 || std::isdigit(atts[3][0])==false || 
      std::isdigit(atts[2][0])==false)
    std::cout << "Something went worng! Time events could not be added.\n";
  else {
    std::cout << "Added to context: " << atts[1] << std::endl;
    p->getContext(atts[1])->add_timeEvent("t_throwEvent", atts[1], atts[0], 
        std::stod(atts[3], nullptr), stoi(atts[2]));
  }

  m_curPermeable = false;
}

void Context::h_setNewQuest(std::string& sIdentifier, CPlayer* p) {
  p->setNewQuest(sIdentifier);
  m_curPermeable = false;
}

void Context::h_changeDialog(std::string& sIdentifier, CPlayer* p) {
  if (sIdentifier.find("|") == std::string::npos) {
		std::cout << cRED << "h_changeDialog: Arguments to change dialog must be \"char_id|dialog_id\"" 
			<< cCLEAR << std::endl;
		return;
  }
	std::string char_id = func::split(sIdentifier, "|")[0];
	std::string dialog_id = func::split(sIdentifier, "|")[1];
	if (p->getWorld()->getCharacters().count(char_id) > 0)
  	p->getWorld()->getCharacter(char_id)->setDialog(dialog_id);
	else {
		std::cout << "h_changeDialog: char not found by id: " << char_id << ". Testing from getObjectId." 
			<< std::endl;
		auto lambda = [](CPerson* person) { return person->name(); };
		std::string fuzzy_found_id = func::getObjectId(p->getRoom()->getCharacters(), char_id, lambda);
		if (p->getWorld()->getCharacters().count(fuzzy_found_id) > 0) 
  		p->getWorld()->getCharacter(fuzzy_found_id)->setDialog(dialog_id);
		else 
			std::cout << cRED << "h_changeDialog: char found by fuzzy_found_id: " << fuzzy_found_id
				<< cCLEAR << std::endl;
	}
  m_curPermeable = false;
}


void Context::h_changeRoomSilent(std::string& sIdentifier, CPlayer* p) {
  CRoom* room = p->getWorld()->getRoom(sIdentifier);
  if(room != nullptr)
      p->setRoom(p->getWorld()->getRoom(sIdentifier));
  m_curPermeable = false;
}

void Context::h_changeRoom(std::string& sIdentifier, CPlayer* p) {
  CRoom* room = p->getWorld()->getRoom(sIdentifier);
  if(room != nullptr)
      p->changeRoom(p->getWorld()->getRoom(sIdentifier));
  m_curPermeable = false;
}

// ***** ***** STANDARD CONTEXT ***** ***** //

void Context::h_ignore(std::string&, CPlayer*) { }

void Context::h_show(std::string& sIdentifier, CPlayer* p) {
  if(sIdentifier == "exits" || sIdentifier == "ausgänge") {
    std::cout << "Calling: " << " show exits!" << std::endl;
    p->appendDescPrint(p->getRoom()->showExits()+"\n");
  }
  else if(sIdentifier == "visited" || sIdentifier == "besuchte räume")
    p->showVisited();
  else if(sIdentifier == "people" || sIdentifier == "personen") {
    p->appendDescPrint(p->getRoom()->showCharacters() + "\n"); 
  }
  else if(sIdentifier == "room")
    p->appendPrint(p->getRoom()->showDescription(p->getWorld()->getCharacters()));
  else if(sIdentifier == "items" || sIdentifier == "gegenstände")
    p->appendDescPrint(p->getRoom()->showItems() + "\n");
  else if(sIdentifier == "details" || sIdentifier == "mobiliar")
    p->appendDescPrint(p->getRoom()->showDetails() + "\n");
  else if(sIdentifier == "inventory" || sIdentifier == "inventar")
    p->appendPrint(p->getInventory().printInventory());
  else if(sIdentifier == "equiped" || sIdentifier == "ausrüstung")
    p->printEquiped();
  else if(sIdentifier == "quests")
    p->showQuests(false);
  else if(sIdentifier == "solved quests" || sIdentifier == "gelöste quests")
    p->showQuests(true);
  else if(sIdentifier == "stats" || sIdentifier == "attribute")
    p->showStats();
  else if(sIdentifier == "mind")
    p->showMinds();
  else if(sIdentifier == "attacks" || sIdentifier == "attacken")
    p->appendPrint(p->printAttacks());
  else if(sIdentifier == "all" || sIdentifier == "alles")
    p->appendDescPrint(p->getRoom()->showAll());
  else
    p->appendErrorPrint("Unbekannte \"Zeige-Funktion\"\n"); 
}

void Context::h_look(std::string& sIdentifier, CPlayer* p) {
  // Extract details (sWhat) and where to look (sWhere) from identifier
  size_t pos = sIdentifier.find(" ");
  if (pos == std::string::npos) {
    p->appendErrorPrint("Ich weiß nicht, was du durchsuchen willst.\n");
    return;
  }
  
  std::string sWhere = sIdentifier.substr(0, pos);
  std::string sWhat = sIdentifier.substr(pos+1);
  auto lambda = [](CDetail* detail) { return detail->name();};
  std::string sDetail = func::getObjectId(p->getRoom()->getDetails(), sWhat, lambda);

  // Check whether input is correct/ detail could be found.
  if (sDetail == "") {
    p->appendErrorPrint("Ich weiß nicht, was du durchsuchen willst.\n");
    return;
  }

  // Check whether sWhere matched with detail
  CDetail* detail  = p->getRoom()->getDetails()[sDetail];
  if(detail->getLook() == sWhere)
    p->appendDescPrint(p->getRoom()->look(sDetail));
  else {
    p->appendErrorPrint("Ich kann nicht " + sWhere + " " + detail->name() 
        + " schauen.\nSoll ich in, auf oder unter " + detail->name() + " schauen?\n");
  }
}

void Context::h_search(std::string& sIdentifier, CPlayer* p) {
	std::cout << "h_search: " << sIdentifier << std::endl;
	auto room = p->getRoom();

	const auto [name, pos] = GetNthObject(sIdentifier);

  auto lambda = [](CDetail* detail) { return detail->name();};
  std::string sDetail = func::getObjectId(room->getDetails(), name, lambda, pos);


  // If detail not found, check for fainted character.
  if (sDetail == "") {
		auto lambda = [](CPerson* person) { return person->name();};
  	std::string char_id = func::getObjectId(room->getCharacters(), name, lambda, pos);
		std::cout << "h_search: found char: " << char_id << std::endl;
		if (char_id != "" && room->getCharacters().at(char_id)->fainted()) {
			p->appendDescPrint(room->LootChar(char_id));
		}
		else 
    	p->appendErrorPrint("Ich weiß nicht, was du durchsuchen willst.\n");
    return;
  }
  // Check whether location is neccessary
  CDetail* detail  = room->getDetails()[sDetail];
  if(detail->getLook() == "")
    p->appendDescPrint(room->look(sDetail));
  else
    p->appendDescPrint("Ich weiß nicht, wo ich suchen soll. Sag: \"schaue "
        "[in/ unter/ ...] [gegenstand]\"\n");
}

void Context::h_goTo(std::string& sIdentifier, CPlayer* p) {
  p->changeRoom(sIdentifier);
}

void Context::h_startDialog(std::string& sIdentifier, CPlayer* p) {
  //Get selected character
  auto lambda1 = [](CPerson* person) { return person->name(); };
  std::string character = func::getObjectId(p->getRoom()->getCharacters(), sIdentifier, lambda1);
  auto lambda2 = [](CPlayer* player) { return player->name(); };
  std::string player = func::getObjectId(p->getMapOFOnlinePlayers(), sIdentifier, lambda2);

  //Check if character was found
  if (character != "") 
    p->startDialog(character);  
  else if (player != "" && p->getWorld()->muliplayer()) 
    p->startChat(p->getPlayer(player));
  else
    p->appendErrorPrint("Character not found");
}

void Context::h_startDialogDirect(std::string &sIdentifier, CPlayer *p) {
  std::cout << "h_startDialogDirect: " << sIdentifier << std::endl;

  std::string aim_identified = m_jAttributes["infos"]["h_startDialogDirect"]["identifier"];
  std::string character = m_jAttributes["infos"]["h_startDialogDirect"]["character"];

  // Check if identifier matches entered identifier.
  if (aim_identified != sIdentifier) {
    return;
  }

  // Check if given character exists. If he does, call dialog.
  if (p->getWorld()->getCharacter(character) != nullptr) {
    p->startDialog(character, p->getWorld()->getCharacter(character)->getDialog());
  }
}

void Context::h_startDialogDirectB(std::string &sIdentifier, CPlayer *p) {
  std::cout << "h_startDialogDirektB: " << sIdentifier << std::endl;
  // Check if given character exists. If he does, call dialog.
  if (p->getWorld()->getCharacter(sIdentifier) != nullptr) {
    p->startDialog(sIdentifier, p->getWorld()->getCharacter(sIdentifier)->getDialog());
  }
  m_curPermeable = false;
}


void Context::h_changeSound(std::string &sIdentifier, CPlayer *p) {
  std::cout << "h_changeSound: " << sIdentifier << std::endl;
  // Change overall music, if no seperator.
  p->set_music(sIdentifier);
  p->updateMedia();
  m_curPermeable = false;
}

void Context::h_changeImage(std::string &sIdentifier, CPlayer *p) {
  std::cout << "h_changeImage: " << sIdentifier << std::endl;
  // Change overall music, if no seperator.
  p->set_image(sIdentifier);
  p->updateMedia();
  m_curPermeable = false;
}

void Context::h_moveCharToRoom(std::string &sIdentifier, CPlayer *p) {
	std::cout << "h_moveCharToRoom: " << sIdentifier << std::endl;
	std::string msg;
	std::string char_id;
	auto [from_room, to_room] = GetRoomsAndObjectID(sIdentifier, msg, char_id, p);
	if (!from_room)
		std::cout << cRED << "h_moveCharToRoom failed: " << msg << cCLEAR << std::endl;
	else if (from_room->getCharacters().count(char_id) == 0) {
		std::cout << cRED << "h_moveCharToRoom failed: char does not exist: " << char_id << cCLEAR << std::endl;
	}
	else if (to_room->getCharacters().count(char_id) > 0) {
		std::cout << cRED << "h_moveCharToRoom failed: char " << char_id << " already exists in " << to_room->id() << cCLEAR << std::endl;
	}
	else {
		to_room->getCharacters()[char_id] = from_room->getCharacters()[char_id];
		from_room->getCharacters().erase(char_id);
	}
  m_curPermeable = false;
}

void Context::h_moveDetailToRoom(std::string &sIdentifier, CPlayer *p) {
	std::cout << "h_moveDetailToRoom: " << sIdentifier << std::endl;
	std::string msg;
	std::string detail_id;
	auto [from_room, to_room] = GetRoomsAndObjectID(sIdentifier, msg, detail_id, p);
	if (!from_room)
		std::cout << cRED << "h_moveDetailToRoom failed: " << msg << cCLEAR << std::endl;
	else if (from_room->getDetails().count(detail_id) == 0) {
		std::cout << cRED << "h_moveDetailToRoom failed: char does not exist: " << detail_id << cCLEAR << std::endl;
	}
	else if (to_room->getDetails().count(detail_id) > 0) {
		std::cout << cRED << "h_moveDetailToRoom failed: char already exists: " << detail_id << cCLEAR << std::endl;
	}
	else {
		to_room->getDetails()[detail_id] = from_room->getDetails()[detail_id];
		from_room->getDetails().erase(detail_id);
	}
  m_curPermeable = false;
}

void Context::h_moveItemToRoom(std::string &sIdentifier, CPlayer *p) {
	std::cout << "h_moveItemToRoom: " << sIdentifier << std::endl;
	std::string msg;
	std::string item_id;
	auto [from_room, to_room] = GetRoomsAndObjectID(sIdentifier, msg, item_id, p);
	if (!from_room)
		std::cout << cRED << "h_moveItemToRoom failed: " << msg << cCLEAR << std::endl;
	else if (from_room->getItems().count(item_id) == 0) {
		std::cout << cRED << "h_moveItemToRoom failed: char does not exist: " << item_id << cCLEAR << std::endl;
	}
	else if (to_room->getItems().count(item_id) > 0) {
		std::cout << cRED << "h_moveItemToRoom failed: char already exists: " << item_id << cCLEAR << std::endl;
	}
	else {
		to_room->getItems()[item_id] = from_room->getItems()[item_id];
		from_room->getItems().erase(item_id);
	}
  m_curPermeable = false;
}

std::pair<CRoom*, CRoom*> Context::GetRoomsAndObjectID(const std::string& sIdentifier, std::string& msg, 
			std::string& obj_id, CPlayer* p) {
	std::cout << "Helper called: RemoveObjectFromRoom: " << sIdentifier << std::endl;
	std::pair<CRoom*, CRoom*> res = {nullptr, nullptr};
  if (sIdentifier.find("|") == std::string::npos) {
		msg = "object to remove or room is missing. Sperate both with \"|\".";
	}
	auto parts = func::split(sIdentifier, "|");
	if (parts.size() < 2) {
		msg = "identifier is missing some information, must be \"char|room_from|room_to\".";
	}

	try {
		obj_id = func::split(sIdentifier, "|")[0];
		std::string room_from_id = func::split(sIdentifier, "|")[1];
		std::string room_to_id = func::split(sIdentifier, "|")[2];
		std::cout << "GetRoomsAndObjectID : got " << obj_id << ", " << room_from_id << ", " << room_to_id << std::endl;
  	CRoom* from_room = p->getWorld()->getRoom(room_from_id);
  	CRoom* to_room = p->getWorld()->getRoom(room_to_id);
		if (!from_room)
			msg = "room from which to move does not exist: " + room_from_id;
		else if (!to_room)
			msg = "room from to to move does not exist: " + room_to_id;
		else
			res = {from_room, to_room};
	} catch (std::exception& e) {
    msg = e.what();
	}
	return res;
}

void Context::h_addCharToRoom(std::string &sIdentifier, CPlayer *p) {
	std::cout << "h_addCharToRoom: " << sIdentifier << std::endl;
	std::string msg;
	auto [room, char_id] = GetRoomAndObjectID(sIdentifier, msg, p);
	if (!room)
		std::cout << cRED << "h_addCharToRoom failed: " << msg << cCLEAR << std::endl;
	else {
		auto character = p->getWorld()->GetNewChar(char_id, room->id(), p);
		if (!character)
			std::cout << cRED << "h_addCharToRoom failed: detail (template) does not exist: " << char_id << cCLEAR << std::endl;
		else 
			room->getCharacters()[character->id()] = character;
	}
	m_curPermeable = false;
}

void Context::h_addDetailToRoom(std::string &sIdentifier, CPlayer *p) {
	std::cout << "h_addDetailToRoom: " << sIdentifier << std::endl;
	std::string msg;
	auto [room, detail_id] = GetRoomAndObjectID(sIdentifier, msg, p);
	if (!room)
		std::cout << cRED << "h_addDetailToRoom failed: " << msg << cCLEAR << std::endl;
	else {
		auto detail = p->getWorld()->GetNewDetail(detail_id, room->id(), p);
		if (!detail)
			std::cout << cRED << "h_addDetailToRoom failed: detail (template) does not exist: " << detail_id << cCLEAR << std::endl;
		else 
			room->getDetails()[detail->id()] = detail;
	}
  m_curPermeable = false;
}

void Context::h_addItemToRoom(std::string &sIdentifier, CPlayer *p) {
	std::cout << "h_addItemToRoom: " << sIdentifier << std::endl;
	std::string msg;
	auto [room, item_id] = GetRoomAndObjectID(sIdentifier, msg, p);
	if (!room)
		std::cout << cRED << "h_addItemToRoom failed: " << msg << cCLEAR << std::endl;
	else if (p->getWorld()->items().count(item_id) == 0)
		std::cout << cRED << "h_addItemToRoom failed: item (template) does not exist: " << item_id << cCLEAR << std::endl;
	else {
		auto [item_json, item_room_id] = p->getWorld()->GetUpdatedTemplate(room->id(), item_id, {}, p->getWorld()->items(), {});
		room->getItems()[item_room_id] = new CItem(item_json, p);
	}
  m_curPermeable = false;
}

// *** remove objects from room  *** //
void Context::h_removeCharFromRoom(std::string &sIdentifier, CPlayer *p) {
	std::cout << "h_removeCharFromRoom: " << sIdentifier << std::endl;
	std::string msg;
	auto [room, char_id] = GetRoomAndObjectID(sIdentifier, msg, p);
	if (!room)
		std::cout << cRED << "h_removeCharFromRoom failed: " << msg << cCLEAR << std::endl;
	else if (room->getCharacters().count(char_id) == 0)
		std::cout << cRED << "h_removeCharFromRoom failed: character does not exist. " << char_id << cCLEAR << std::endl;
	else 
		room->getCharacters().erase(char_id);
	m_curPermeable = false;
}

void Context::h_removeDetailFromRoom(std::string &sIdentifier, CPlayer *p) {
	std::cout << "h_removeDetailFromRoom: " << sIdentifier << std::endl;
	std::string msg;
	auto [room, detail_id] = GetRoomAndObjectID(sIdentifier, msg, p);
	if (!room)
		std::cout << cRED << "h_removeDetailFromRoom failed: " << msg << cCLEAR << std::endl;
	else if (room->getDetails().count(detail_id) == 0)
		std::cout << cRED << "h_removeDetailFromRoom failed: detail does not exist. " << detail_id << cCLEAR << std::endl;
	else 
		room->getDetails().erase(detail_id);
	m_curPermeable = false;
}

void Context::h_removeItemFromRoom(std::string &sIdentifier, CPlayer *p) {
	std::cout << "h_removeItemFromRoom: " << sIdentifier << std::endl;
	std::string msg;
	auto [room, item_id] = GetRoomAndObjectID(sIdentifier, msg, p);
	if (!room)
		std::cout << cRED << "h_removeDetailFromRoom failed: " << msg << cCLEAR << std::endl;
	else if (room->getItems().count(item_id) == 0) 
		std::cout << cRED << "h_removeItemFromRoom failed: item does not exist. " << item_id << cCLEAR << std::endl;
	else 
		room->getItems().erase(item_id);
	m_curPermeable = false;
}

std::pair<CRoom*, std::string> Context::GetRoomAndObjectID(const std::string& sIdentifier, std::string& msg, CPlayer* p) {
	std::cout << "Helper called: RemoveObjectFromRoom: " << sIdentifier << std::endl;
	std::pair<CRoom*, std::string> res = {nullptr, ""};
  if (sIdentifier.find("|") == std::string::npos) {
		msg = "object to remove or room is missing. Sperate both with \"|\".";
	}
	try {
		std::string obj_id = func::split(sIdentifier, "|")[0];
		std::string room_id = func::split(sIdentifier, "|")[1];
		std::cout << "RemoveObjectFromRoom: got " << obj_id << ", " << room_id << std::endl;
  	CRoom* room = p->getWorld()->getRoom(room_id);
		if (!room)
			msg = "room does not exist: " + room_id;
		else
			res = {room, obj_id};
	} catch (std::exception& e) {
    msg = e.what();
	}
	return res;
}


void Context::h_removeHandler(std::string &sIdentifier, CPlayer *p) {
  // Get attributes and check size.
  auto attributes = func::split(sIdentifier, "|");
  if (attributes.size() < 3) {
    std::cout << cRED << "h_removeHandler: location, location-id or listener-id are missing: " 
      << sIdentifier << cCLEAR << std::endl;
    return;
  }
  // Remove handler from location.
  p->RemoveListenerFromLocation(attributes[0], attributes[1], attributes[2]);
  m_curPermeable = false;
}

void Context::h_take(std::string& sIdentifier, CPlayer* p) {
  if (sIdentifier.find("all") == 0)
    p->addAll();
  else if (p->getRoom()->getItem(sIdentifier) == NULL) {
    std::cout << "Not found.\n";
    p->appendErrorPrint("Item not found.\n");
  }
  else {
    std::cout << "Getting item.\n";
    p->addItem(p->getRoom()->getItem(sIdentifier));
  }
}

void Context::h_consume(std::string& sIdentifier, CPlayer* p) {
	UseItem(sIdentifier, "consume", p);
}

void Context::h_read(std::string& sIdentifier, CPlayer* p) {
	UseItem(sIdentifier, "read", p);
}

void Context::h_equipe(std::string& sIdentifier, CPlayer* p) {
	UseItem(sIdentifier, "equipe", p);
}
void Context::h_use(std::string& sIdentifier, CPlayer* p) {
	UseItem(sIdentifier, "", p);
}

void Context::UseItem(std::string& identifier, std::string category, CPlayer* p) {
	auto item = p->getInventory().getItem(identifier);
	if (!item)
    p->appendErrorPrint(p->getWorld()->GetSTDText("item_not_in_inventory") + "\n");
	else {
		if (category != "" && item->getCategory() != category)
      p->appendErrorPrint(p->getWorld()->GetSTDText(category  + "_error"));
		else
			item->callFunction(p);
	}
}

void Context::h_dequipe(std::string& sIdentifier, CPlayer* p) {
  p->dequipeItem(sIdentifier);
}

void Context::h_examine(std::string &sIdentifier, CPlayer*p) {
  // Check for room 
  if (sIdentifier == "room" || sIdentifier == "raum") {
    p->appendPrint(p->getRoom()->showDescription(p->getWorld()->getCharacters()));
    return;
  }

  // Check for detail
  auto lambda1 = [](CDetail* detail) { return detail->name(); };
  std::string sObject = func::getObjectId(p->getRoom()->getDetails(), sIdentifier, lambda1);
  if (sObject != "")
    p->appendPrint(p->getRoom()->getDetails()[sObject]->text());

  auto lambda2 = [](CItem* item) { return item->name(); };
  sObject = func::getObjectId(p->getRoom()->getItems(), sIdentifier, lambda2);
  // Check for item
  if (sObject != "")
    p->appendPrint(p->getRoom()->getItems()[sObject]->text());

  // Check for person
  auto lambda3 = [](CPerson* person) { return person->name();};
  sObject = func::getObjectId(p->getRoom()->getCharacters(), sIdentifier, lambda3);
  if (sObject != "")
    p->appendPrint(p->getWorld()->getCharacter(sObject)->text());

  // Check for inventory
  if (p->getInventory().getItem(sIdentifier) != NULL)
    p->appendPrint(p->getInventory().getItem(sIdentifier)->text());
}

void Context::h_test(std::string& sIdentifier, CPlayer* p) {
  p->appendPrint(sIdentifier);
}


// **** SPECIAL HANDLER ***** //

void Context::h_moveToHospital(std::string& sIdentifier, CPlayer* p) {
  //Get selected room
  auto lambda = [](CExit* exit) { return exit->prep() + "_" + exit->name();};
  if (p->getRoom()->id().find("zug.compartment") == std::string::npos 
      || func::getObjectId(p->getRoom()->getExtits(), sIdentifier, lambda) != "zug.trainCorridor")
    return;

  p->changeRoom(p->getWorld()->getRoom("hospital.foyer"));
  m_block = true;
  m_curPermeable=false;
}

void Context::h_exitTrainstation(std::string& sIdentifier, CPlayer* p)
{
    std::cout << "h_exitTrainstation, " << sIdentifier << std::endl;

    auto lambda= [](CExit* exit) { return exit->prep() + " " + exit->name(); };
    if(p->getRoom()->id() != "trainstation.eingangshalle" || func::getObjectId(p->getRoom()->getExtits(), sIdentifier, lambda) != "trainstation.ausgang")
        return;

    p->appendStoryPrint("Du drehst dich zum dem großen, offen stehenden Eingangstor der Bahnhofshalle. Und kurz kommt dir der Gedanke doch den Zug nicht zu nehmen, doch alles beim Alten zu belassen. Doch etwas sagt dir, dass es einen guten Grund gab das nicht zu tun, einen guten Grund nach Moskau zu fahren. Und auch, wenn du ihn gerade nicht mehr erkennst. Vielleicht ist gerade das der beste Grund: rausfinden, was dich dazu getrieben hat, diesen termin in Moskau zu vereinbaren.\n Du guckst dich wieder in der Halle um, und überlegst, wo du anfängst zu suchen.\n");
    
    m_block=true;
}

void Context::h_thieve(std::string& sIdentifier, CPlayer* p) {
  if (m_jAttributes["infos"].count("h_thieve") > 0) {
    std::string str = m_jAttributes["infos"]["h_thieve"];
    std::cout << "h_thieve str: " << str << std::endl;
    p->startDialog(str, p->getWorld()->getCharacter(str)->getDialog("thieve"));
  }

  else
    p->appendStoryPrint("You know, me son. You should not be stealing!");
  m_curPermeable = false;
}

void Context::h_attack(std::string& sIdentifier, CPlayer* p) {
  std::cout << "h_attack" << sIdentifier << std::endl;
  if (m_jAttributes["infos"].count("h_attack") == 0) {
    std::cout << "Infos not found: " << m_jAttributes.dump() << std::endl;
    return; 
  }

  std::string character_id = m_jAttributes["infos"]["h_attack"];
  std::cout << "Infos: " << character_id << std::endl;
  auto character = p->getWorld()->getCharacter(character_id);
  // Only start fight. if character is still alive.
  if (!character->fainted()) {
    p->setFight(new CFight(p, {character}));
    m_curPermeable = false;
  }
}


void Context::h_try(std::string& sIdentifier, CPlayer* p) {
  p->throw_events("go to second", "try");
  p->throw_events("talk to tall", "try");
  p->throw_events("1", "try");
  p->throw_events("go neben", "try");
  p->throw_events("go Toil", "try");
  p->throw_events("go frauen", "try");
  p->throw_events("2", "try");
}


// ***** ***** FIGHT CONTEXT ***** ***** //
void Context::h_fight(std::string& sIdentifier, CPlayer* p) {
  std::string newCommand = p->getFight()->NextTurn((sIdentifier));
  if (newCommand != "")    
    p->throw_events(newCommand, "h_fight");
}

// ***** ***** DIALOG CONTEXT ***** *****

void Context::initializeDialogListeners(std::string new_state, CPlayer* p) {
  //Set (new) state, clear listeners and add help-listener
  setAttribute<std::string>("state", new_state); 
  m_eventmanager.clear();
  AddSimpleListener("h_help", "help", 0);
  AddSimpleListener("h_ignore", "changed_room", 0);

  //Add listener for each dialog option.
  std::vector<size_t> activeOptions = p->getDialog()->getState(new_state)
    ->getActiveOptions(p);
  std::map<size_t, size_t> mapOtptions;
  size_t counter = 1;
  for(auto opt : activeOptions) {
    mapOtptions[counter] = opt;
    AddSimpleListener("h_call", std::to_string(counter), 0);
    counter++;
  }
  setAttribute<std::map<size_t, size_t>>("mapOptions", mapOtptions);

  CDState* state = p->getDialog()->getState(new_state);
  media_["music"] = (state->music() != "") ? state->music() : p->getCurDialogPartner()->music();
  media_["image"] = (state->image() != "") ? state->image() : p->getCurDialogPartner()->image();
  p->updateMedia();
}

void Context::h_call(std::string& sIdentifier, CPlayer* p) {
  size_t option = getAttribute<std::map<size_t, size_t>>("mapOptions")[stoi(sIdentifier)];

  std::string cur_state = getAttribute<std::string>("state");
  std::string next_state = p->getDialog()->getState(cur_state)
    ->getNextState(std::to_string(option), p);
  CDState* state = p->getDialog()->getState(next_state);
 
  // Change sound to room-sound or to player sound, if changed.
  media_["music"] = (state->music() != "") ? state->music() : p->getCurDialogPartner()->music();
  media_["image"] = (state->image() != "") ? state->image() : p->getCurDialogPartner()->image();
  p->updateMedia();

  if(next_state == "trade")
    p->startTrade(getAttribute<std::string>("partner"));
  else {
    initializeDialogListeners(next_state, p);
    std::string newCommand = p->getDialog()->getState(next_state)->callState(p);
    if(newCommand != "")
      p->throw_events(newCommand, "h_call");
  }
}

// ***** ***** TRADE CONTEXT ***** ***** // 

void Context::print(CPlayer* p) {
  CPerson* partner = p->getWorld()->getCharacter(getAttribute<std::string>("partner"));
  p->appendPrint("<b>" + p->name() + "s Inventar:</b>\n" 
                  + p->getInventory().printInventory()
                  + "\n<b>" + partner->name() + "s Inventar: </b>\n" 
                  + partner->getInventory().printInventory());
}

void Context::h_sell(std::string& sIdentifier, CPlayer* p) {
  CPerson* partner = p->getWorld()->getCharacter(getAttribute<std::string>("partner"));
  CItem* curItem = p->getInventory().getItem(sIdentifier);
  if(curItem == NULL) {
    p->appendErrorPrint("Dieser Gegenstand befindet sich nicht in deinem "
        "Inventar, du kannst ihn logischerwiese dehalb nicht verkaufen!\n");
  }
  else if(p->getEquipment().count(curItem->kind()) > 0 
      && p->getEquipment()[curItem->kind()]->id() == curItem->id()) {
    p->appendErrorPrint("Du kannst ausgerüstete Gegenstände nicht "
        "verkaufen.\n");
  }
  else {
    p->appendDescPrint("Du hast " + curItem->name() + " verkauft.\n\n");
    p->Update(curItem->costs());
    CItem* item = new CItem(curItem->getAttributes(), p);
    partner->getInventory().addItem(item);
    p->getInventory().removeItemByID(curItem->id());
  }
  print(p);
}

void Context::h_buy(std::string& sIdentifier, CPlayer* p) {
  CPerson* partner = p->getWorld()->getCharacter(getAttribute<std::string>("partner"));
  CItem* curItem = partner->getInventory().getItem(sIdentifier);

  if(curItem == NULL)
    p->appendErrorPrint("Dieser Gegenstand befindet sich nicht in dem Inventar "
        "des Händlers, du kannst ihn logischerwiese dehalb nicht kaufen!\n");
  else {
    if (p->CompareUpdates(curItem->costs().Reverse())) {
      p->appendErrorPrint("Costs of item would exeed limits: " + curItem->name());
      return;
    }

    p->appendDescPrint("Du hast " + curItem->name() + " gekauft.\n\n");
    p->Update(curItem->costs().Reverse());
    p->getInventory().addItem(new CItem(curItem->getAttributes(), p));
    partner->getInventory().removeItemByID(curItem->id());
  }
  print(p);
}

void Context::h_exit(std::string&, CPlayer* p) {
  CPerson* partner = p->getWorld()->getCharacter(getAttribute<std::string>("partner"));
  p->getContexts().erase("trade");
  p->getContexts().erase("dialog");
  p->startDialog(partner->id());
}

// ***** ***** CHAT CONTEXT ***** ***** //
void Context::h_send(string& sInput, CPlayer* p) {
  CPlayer* chatPartner = p->getPlayer(getAttribute<std::string>("partner"));
  chatPartner->send(p->returnSpeakerPrint(func::returnToUpper(p->name()), sInput + "\n"));
  p->appendSpeackerPrint("YOU", sInput + "\n");
}

void Context::h_end(string& sMessage, CPlayer* p) {
  CPlayer* chatPartner = p->getPlayer(getAttribute<std::string>("partner"));
  if(sMessage == "[end]")
      chatPartner->send("[Gespräch beendet].\n");
  else {
    chatPartner->send(p->returnSpeakerPrint(func::returnToUpper(p->name()), sMessage));
    chatPartner->send("[Gespräch beendet].\n");
  }

  chatPartner->getContexts().erase("chat");

  p->appendPrint("Gespräch mit " + chatPartner->name() + " beendet.\n");
  p->getContexts().erase("chat");
  m_block = true; 
}


// ***** ***** CHAT CONTEXT ***** ***** //

void Context::h_next(std::string& sIdentifier, CPlayer* p) {
  std::cout << "h_next\n";
  std::cout << "Item: " << m_jAttributes["item"] << std::endl;
  CItem* item = p->getInventory().getItem_byID(m_jAttributes["item"]);

  std::cout << "Mark: " << m_jAttributes["mark"] << std::endl;
  std::cout << "Pages.size(): " << item->getPages()->getNumPages() << std::endl;
  if(m_jAttributes["mark"] == item->getPages()->getNumPages()-1) {
    p->appendDescPrint("Das Buch ist zuende.");
		h_stop(sIdentifier, p);
	}
  else {
    m_jAttributes["mark"] = m_jAttributes["mark"].get<size_t>()+1;
    p->appendPrint(
          "Seite: " + std::to_string(m_jAttributes["mark"].get<int>()+1) + ": \n"
        + item->getPages()->pagePrint(m_jAttributes["mark"]) + "\n");
  }
}

void Context::h_prev(std::string&, CPlayer* p) {
  std::cout << "h_prev\n";
  std::cout << "Item: " << m_jAttributes["item"] << std::endl;
  CItem* item = p->getInventory().getItem_byID(m_jAttributes["item"]);
  if(m_jAttributes["mark"] == 0)
    p->appendDescPrint("Das Buch ist bereits auf der ersten Seite aufgeschlagen.");
  else {
    m_jAttributes["mark"] = m_jAttributes["mark"].get<size_t>() -1;
    p->appendPrint(
          "Seite: " + std::to_string(m_jAttributes["mark"].get<int>()+1) + ": \n"
        + item->getPages()->pagePrint(m_jAttributes["mark"]) + "\n");
  }
}

void Context::h_mark(std::string&, CPlayer* p) {
  std::cout << "h_mark\n";
  std::cout << "Item: " << m_jAttributes["item"] << std::endl;

  p->getInventory().getItem_byID(m_jAttributes["item"])->setMark(m_jAttributes["mark"]);
  size_t mark = m_jAttributes["mark"];
  p->appendPrint("Leesezeichen auf Seite " + std::to_string(mark+1) + " gesetzt.");
}

void Context::h_underline(std::string& sIdentifier, CPlayer* p) {
  if(sIdentifier.find(",") == std::string::npos) {
    p->appendErrorPrint("Leider nicht das richtige Format \"[von], [bis]\"");
    return;
  }
  std::string str1 = func::split(sIdentifier, ", ")[0];
  std::string str2 = func::split(sIdentifier, ", ")[1];
  
  CItem* item = p->getInventory().getItem_byID(m_jAttributes["item"]);
  bool success = item->getPages()->underline(m_jAttributes["mark"], str1, str2);
  if(success == false)
      p->appendErrorPrint("Eines der Wörter konnte nicht gefunden werden.");
  else
      p->appendPrint(item->getPages()->pagePrint(m_jAttributes["mark"]));
}

void Context::h_stop(std::string&, CPlayer* p) {
  p->appendDescPrint("Ich habe das Buch wieder zugeschlagen.\n");
  p->getContexts().erase("read"); 
}


// ***** ***** QUEST CONTEXT ***** ***** //

void Context::h_react(std::string& sIdentifier, CPlayer* p) {
  //Obtain quest and quest-step
  CQuest* quest = p->getWorld()->getQuest(getAttribute<std::string>("questID"));

  for (auto it : quest->getSteps()) {
    std::cout << "h_react Checking: " << it.second->logic() << ": " << m_curEvent.first << "|" << sIdentifier << std::endl;
    p->set_subsitues({{"cmd", m_curEvent.first}, {"input", sIdentifier}});
    LogicParser logic(p->GetCurrentStatus());
		bool success = false;
		try {
			success = logic.Success(it.second->logic());
		} catch(std::exception& e) {
			std::cout << "h_react logic.Success failed: " << e.what() << std::endl;
		}
		std::cout << "h_react room matches input: " << (it.second->logic().find(sIdentifier) != std::string::npos) 
				<< std::endl;
		std::cout << "h_react Checking: " << success << std::endl;
    if (it.second->getSolved() == false && success == true) { 
			std::cout << "h_react Marking quest solved." << std::endl;
      p->questSolved(quest->getID(), it.first);
    }
  }
}


// *** *** Tutorial *** *** //

/*
void Context::h_startTutorial(std::string&, CPlayer* p)
{
    p->appendStoryPrint("Willkommen Neuling. Willkommen zu \"DER ZUG\". Du weiß vermutlich nicht, wer du bist und wo du hin willst und musst und kannst. Du weißt noch nicht, was <i>hier</i> ist, was <i>jetzt</i> ist.$");
    p->appendBlackPrint("Um dich zurecht zu finden. Die dir noch offene Fragen langsam <i>für dich</i> zu beantworten, nutze zunächst die Befehle \"zeige Ausgänge\", \"zeige Personen\", \"zeige Details\".$");
 
    p->appendStoryPrint("Willkommen bei \"DER ZUG\"! Du befindest dich auf dem Weg nach Moskau. Dir fehlt dein Ticket. Tickets sind teuer. Glücklicherweise kennst du einen leicht verrückten, viel denkenden Mann, der sich \"Der Ticketverkäufer\" nennt. Suche ihn, er hat immer noch ein günstiges Ticket für dich.$");
    p->appendBlackPrint("Benutze die Befehle \"gehe [name des Ausgangs]\", um den Raum zu wechseln, um dir Personen und Ausgänge anzeigen zu lassen, nutze \"Zeige Personen\", bzw. \"Zeige Ausgänge\" oder auch \"zeige alles\". Eine Liste mit allen Befehlen und zusätzlichen Hilfestellungen erhältst du, indem du \"help\" eingibst.$");
 
    //p->setNewQuest("zug_nach_moskau");
    m_curPermeable = false;
}
*/


// *** *** Die komische Gruppe *** *** //
void Context::h_reden(std::string& sIdentifier, CPlayer* p) {
  auto lambda = [](CPerson* person) { return person->name();};
  std::string character = func::getObjectId(p->getRoom()->getCharacters(), 
      sIdentifier, lambda);
  if(character == "" || character.find("passanten_gruppe") == std::string::npos)
    return;

  CQuest* quest = p->getWorld()->getQuest(getAttribute<std::string>("questID"));
  CQuestStep* step = quest->getSteps()["reden"];
  
  //If character ID ends with a number, delete.
  if(std::isdigit(character.back()) == true)
    character.pop_back();

  for(size_t i=0; i<step->getWhich().size(); i++) {
    if(step->getWhich()[i].find(character) != std::string::npos)
      return;
  }

  step->getWhich().push_back(character);
  step->incSucc(1);
  p->questSolved(quest->getID(), "reden");

  //Change dialog of all "Passanten"
  if(step->getSolved() == true) {
    std::cout << "CHANGING DIALOGS OF PASSANTEN\n";
    p->getWorld()->getCharacter("trainstation.eingangshalle.passanten_gruppe")
        ->setDialog("2");
    p->getWorld()->getCharacter("trainstation.nebenhalle.passanten_gruppe")
        ->setDialog("2");
    p->getWorld()->getCharacter("trainstation.gleis5.passanten_gruppe")
        ->setDialog("2");
    p->getContexts().erase(quest->getID());
  }
}

// *** *** GELD AUFTREIBEN *** *** //
void Context::h_geldauftreiben(std::string& sIdentifier, CPlayer* p) {
  if (p->getStat("gold") + stoi(sIdentifier) < 10)
    return;

  CQuest* quest = p->getWorld()->getQuest(getAttribute<std::string>("questID"));
  if (quest->getActive() == true)
    p->appendDescPrint("Wundervoll, genug Geld, um das Ticket zu kaufen!\n");

  p->questSolved(quest->getID(), "geldauftreiben");
  p->getContexts().erase(quest->getID());
}


// ***** ***** OTHER CONTEXTS ***** ***** //

void Context::h_select(std::string& sIdentifier, CPlayer* p)
{
    map_type map_objects = getAttribute<map_type>("map_objects");
    std::string obj = func::getObjectId(map_objects, sIdentifier);

    p->throw_events(getAttribute<std::string>("eventtype")+ " " + obj, "h_select");
    m_curPermeable=false;
    p->getContexts().erase("select"); 
}

void Context::h_choose_equipe(std::string& sIdentifier, CPlayer* p) {
  if(sIdentifier == "yes" || sIdentifier == "ja") {
		auto item = p->getInventory().getItem_byID(getAttribute<std::string>("itemID"));
		if (item) {
			p->dequipeItem(item->kind());
			p->equipeItem(item, item->kind());
			p->getContexts().erase("equipe");
		}
		else {
    	p->appendTechPrint("Sorry, something went wrong: can't find item with this id. "
					"try dequiping and then equiping item.");
    	p->getContexts().erase("equipe");
		}
  }

  else if(sIdentifier == "no" || sIdentifier == "nein") {
    p->appendDescPrint("Du entscheidest dich "
        + p->getInventory().getItem_byID(getAttribute<std::string>("itemID"))->name() 
        + " nicht auszurüsten.\n");
    p->getContexts().erase("equipe");
  }

  else
    error(p);
}

void Context::h_levelUp(std::string& identifier, CPlayer* p) {
	if (identifier.find("|") == std::string::npos) {
		std::cout << cRED << "h_levelUp: split num-points and update amount with \"|\"" << cCLEAR << std::endl; 
	}
	else {
		std::string num = func::split(identifier, "|")[0];
		std::string val = func::split(identifier, "|")[1];
		if (!isdigit(num[0]) || !isdigit(val[0])) {
			std::cout << cRED << "h_levelUp: num-points and update amount must bin numbers!" << cCLEAR << std::endl; 
		}
		else {
			p->UpdateStats(std::stoi(num), std::stoi(val));
		}
	}
	m_curPermeable = false;
}

void Context::h_updateStats(std::string& sIdentifier, CPlayer* p) {
	std::cout << "h_updateStats: " << sIdentifier << std::endl;
  //Get ability (by number or by name/ id).
  std::string ability = "";
	const auto& skillable = p->getWorld()->attribute_config().skillable_;
	const auto& attributes = p->getWorld()->attribute_config().attributes_;
  for (size_t i=0; i<skillable.size(); i++) {
    if (fuzzy::fuzzy_cmp(func::returnToLower(attributes.at(skillable[i]).name_), sIdentifier) <= 0.2)
      ability = skillable[i];
    if (std::isdigit(sIdentifier[0]) && i == stoul(sIdentifier, nullptr, 0)-1)
      ability = skillable[i];
  }
	std::cout << "h_updateStats: got ability: " << ability << std::endl;

  // Update ability.
  int num = getAttribute<int>("numPoints")-1;
  int val = getAttribute<int>("value");
	p->SetAttributeAndThrow(ability, p->getStat(ability) + val);
	
	// Add events caused by changing attribute
	auto events = p->getWorld()->attribute_config().attributes_.at(ability).GetExceedBoundEvents(p->getStat(ability));
	if (events.size() > 0) {
		p->addPostEvent(events);
	}


  p->appendSuccPrint(ability + " updated by " + std::to_string(val) + "\n");
  p->getContexts().erase("updateStats");
	if (num > 0)
		p->UpdateStats(num, val);
	else 
		p->appendPrint("Done.");
}

// ----- ***** TIME EVENTS ***** ------ //
void Context::t_setAttribute(std::string& str, CPlayer* p) {
  std::cout << "t_setAttribute " << str << std::endl;
	// Update attribute
	h_setAttribute(str, p);
  std::cout << "t_setAttribute attribute updated " << str << std::endl;

	// Get duration
  double duration = p->getContext("standard")->getTimeEvents().getContext("t_setAttribute")->getDurationMin();
  std::vector<std::string> atts = func::split(str, "|");
	int steps = stoi(atts[4])-1;
	// Delete time event and add new if steps are left
  if (steps > 0) {
    std::cout << "t_setAttribute calling again: " << std::to_string(steps-1)<< std::endl;
		// Reduce one step, join attributes again and add new time event.
		atts[4] = std::to_string(steps);
		std::string infos = func::join(atts, "|");
    std:: cout << infos << ", " << duration << std::endl;
    add_timeEvent("t_setAttribute", "standard", infos, duration);
	}
  str = "delete";
}

void Context::t_throwEvent(std::string& sInfo, CPlayer* p) {
  std::cout << "t_throwEvent: " << sInfo << std::endl;
  std::string str = sInfo;
  p->addPostEvent(str);
  sInfo = "delete";
}

std::pair<std::string, int> Context::GetNthObject(std::string full_name) {
	std::string name = full_name;
	int pos = -1;
	if (name.rfind("-") == name.size()-2 && std::isdigit(name.back())) {
		name = name.substr(0, name.rfind("-"));
		pos = full_name.back() - '0';
		std::cout << "Upated name and pos: " << name << ", " << pos << std::endl;
	}
	return {name, pos};
}
