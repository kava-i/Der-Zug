#include "context.h"
#include "objects/player.h"
#include "game/game.h"

// ***** ***** CONSTRUCTORS ***** ***** //

std::map<std::string, void (Context::*)(std::string&, CPlayer* p)> Context::m_handlers = {};
std::map<std::string, nlohmann::json> Context::m_templates = {};

Context::Context(nlohmann::json jAttributes) {
  m_jAttributes = jAttributes;
  m_sName = jAttributes.value("name", "context");
  m_permeable = jAttributes.value("permeable", false);
  m_curPermeable = m_permeable;
  m_block = false;
  m_sHelp = jAttributes.value("help", "");
  m_sError = jAttributes.value("error", "");

  if(jAttributes.count("handlers") > 0) {
    for(const auto &it : jAttributes["handlers"].get<std::map<string, 
        vector<string>>>()) {
      for(size_t i=0; i<it.second.size(); i++)
        add_listener(it.second[i], it.first, 0-i);
    }
  }
  add_listener("h_help", "help");

  m_error = &Context::error;
}
 
Context::Context(std::string sTemplate) : Context(getTemplate(sTemplate))
{
}

Context::Context(std::string sTemplate, nlohmann::json jAttributes) : Context(getTemplate(sTemplate))
{
    m_jAttributes = jAttributes;
    m_sName = jAttributes.value("name", m_sName);
    m_permeable = jAttributes.value("permeable", m_permeable);
    m_curPermeable = m_permeable;
    m_sHelp = jAttributes.value("help", m_sHelp);
    m_sError = jAttributes.value("error", m_sError);

    if(jAttributes.count("handlers") > 0)
    {
        for(const auto &it : jAttributes["handlers"].get<std::map<string, vector<string>>>()) {
            for(size_t i=0; i<it.second.size(); i++)
                add_listener(it.second[i], it.first, 0-i);
        }
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

nlohmann::json Context::getTemplate(std::string sTemplate)
{
    if(m_templates.count(sTemplate) == 0)
    {
        std::cout << "Template requested that does not exist: " << sTemplate << std::endl;
        return nlohmann::json::object();
    }
    return m_templates[sTemplate];
} 

nlohmann::json& Context::getAttributes() {
    return m_jAttributes;
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

// ***** ***** INITIALIZERS ***** ***** //
void Context::initializeHanlders()
{
    // ***** GENERAL STUFF ***** //
    m_handlers["h_help"] = &Context::h_help;

    // ***** GAME CONTEXT ***** //
    m_handlers["h_reloadGame"] = &Context::h_reloadGame;
    m_handlers["h_reloadPlayer"] = &Context::h_reloadPlayer;
    m_handlers["h_reloadWorlds"] = &Context::h_reloadWorlds;
    m_handlers["h_reloadWorld"] = &Context::h_reloadWorld;
    m_handlers["h_updateListeners"] = &Context::h_updatePlayers;

    // ***** WORLD CONTEXT ***** //
    m_handlers["h_finishCharacter"] = &Context::h_finishCharacter;
    m_handlers["h_killCharacter"] = &Context::h_killCharacter;
    m_handlers["h_deleteCharacter"] = &Context::h_deleteCharacter;
    m_handlers["h_addItem"] = &Context::h_addItem;
    m_handlers["h_recieveMoney"] = &Context::h_recieveMoney;
    m_handlers["h_eraseMoney"] = &Context::h_eraseMoney;
    m_handlers["h_newFight"] = &Context::h_newFight;
    m_handlers["h_endFight"] = &Context::h_endFight;
    m_handlers["h_endDialog"] = &Context::h_endDialog;
    m_handlers["h_gameover"] = &Context::h_gameover;
    m_handlers["h_addQuest"] = &Context::h_addQuest;
    m_handlers["h_showPersonInfo"] = &Context::h_showPersonInfo;
    m_handlers["h_showItemInfo"] = &Context::h_showItemInfo;
    m_handlers["h_changeName"] = &Context::h_changeName;
    m_handlers["h_setAttribute"] = &Context::h_setAttribute;
    m_handlers["h_setNewAttribute"] = &Context::h_setNewAttribute;
    m_handlers["h_addTimeEvent"] = &Context::h_addTimeEvent;
    m_handlers["h_setNewQuest"] = &Context::h_setNewQuest;
    m_handlers["h_changeDialog"] = &Context::h_changeDialog;
    m_handlers["h_printText"] = &Context::h_printText;
    m_handlers["h_changeRoom"] = &Context::h_changeRoom;

    // ***** STANDARD CONTEXT ***** //
    m_handlers["h_showExits"] = &Context::h_showExits;
    m_handlers["h_show"] = &Context::h_show;
    m_handlers["h_look"] = &Context::h_look;
    m_handlers["h_search"] = &Context::h_search;
    m_handlers["h_take"] = &Context::h_take;
    m_handlers["h_goTo"] = &Context::h_goTo;
    m_handlers["h_consume"] = &Context::h_consume;
    m_handlers["h_read"] = &Context::h_read;
    m_handlers["h_equipe"]  = &Context::h_equipe;
    m_handlers["h_dequipe"] = &Context::h_dequipe;
    m_handlers["h_examine"] = &Context::h_examine;
    m_handlers["h_startDialog"] = &Context::h_startDialog;
    m_handlers["h_changeMode"]  = &Context::h_changeMode;
    m_handlers["h_ignore"] = &Context::h_ignore;
    m_handlers["h_try"] = &Context::h_try;

    //m_handlers["h_firstZombieAttack"] = &Context::h_firstZombieAttack;
    m_handlers["h_moveToHospital"] = &Context::h_moveToHospital;
    m_handlers["h_exitTrainstation"] = &Context::h_exitTrainstation;
    m_handlers["h_thieve"] = &Context::h_thieve;
    m_handlers["h_attack"] = &Context::h_attack;

    m_handlers["h_test"] = &Context::h_test;

    // *** FIGHT CONTEXT *** //
    m_handlers["h_fight"] = &Context::h_fight;
    m_handlers["h_fight_show"] = &Context::h_fight_show;

    // *** DIALOG CONTEXT *** //
    m_handlers["h_call"] = &Context::h_call;

    // *** TRADECONTEXT *** //
    m_handlers["h_sell"] = &Context::h_sell;
    m_handlers["h_buy"] = &Context::h_buy;
    m_handlers["h_exit"] = &Context::h_exit;

    // *** CHATCONTEXT *** //
    m_handlers["h_send"] = &Context::h_send;
    m_handlers["h_end"] = &Context::h_end;

    // *** READCONTEXT *** //
    m_handlers["h_stop"] = &Context::h_stop;
    m_handlers["h_next"] = &Context::h_next;
    m_handlers["h_prev"] = &Context::h_prev;
    m_handlers["h_mark"] = &Context::h_mark;
    m_handlers["h_underline"] = &Context::h_underline;

    m_handlers["h_next"] = &Context::h_next;


    // *** QUESTS *** //
    m_handlers["h_react"] = &Context::h_react;
    m_handlers["reden"] = &Context::h_reden;
    m_handlers["geldauftreiben"] = &Context::h_geldauftreiben;

    // *** PROGRAMMER *** //

    // *** OTHER CONTEXT *** //
    m_handlers["h_select"] = &Context::h_select;
    m_handlers["h_choose_equipe"] = &Context::h_choose_equipe;
    m_handlers["h_updateStats"] = &Context::h_updateStats;

    // --- *** TIMEEVENTS *** --- //
    m_handlers["t_highness"] = &Context::t_highness;
    m_handlers["t_throwEvent"] = &Context::t_throwEvent;
}

void Context::initializeTemplates() {
    m_templates["game"] = {
                    {"name", "game"}, {"permeable", true}, 
                    {"handlers", {
                        {"reload_game",{"h_reloadGame"}},
                        {"reload_player",{"h_reloadPlayer"}},
                        {"reload_world", {"h_reloadWorld"}},
                        {"reload_worlds",{"h_reloadWorlds"}},
                        {"update_players",{"h_updatePlayers"}}}}
                    };

    m_templates["first"] = {
                    {"name", "first"}, {"permeable", true},
                    {"handlers", {
                        {"printText", {"h_printText"}},
                        {"show",{"h_show"}}, 
                        {"examine",{"h_examine"}} }}
                    };

    m_templates["world"] = {
                    {"name", "world"}, {"permeable", true},
                    {"handlers", {
                        {"finishCharacter", {"h_finishCharacter"}},
                        {"killCharacter", {"h_killCharacter"}},
                        {"deleteCharacter", {"h_deleteCharacter"}},
                        {"addItem", {"h_addItem"}},
                        {"recieveMoney", {"h_recieveMoney"}},
                        {"eraseMoney", {"h_eraseMoney"}},
                        {"fight", {"h_newFight"}},
                        {"endFight",{"h_endFight"}},
                        {"endDialog",{"h_endDialog"}},
                        {"gameover",{"h_gameover"}},
                        {"addQuest",{"h_addQuest"}},
                        {"showperson",{"h_showPersonInfo"}},
                        {"showitem",{"h_showItemInfo"}},
                        {"changeName",{"h_changeName"}},
                        {"setAttribute", {"h_setAttribute"}},
                        {"setNewAttribute", {"h_setNewAttribute"}}, 
                        {"addTimeEvent", {"h_addTimeEvent"}},
                        {"setNewQuest", {"h_setNewQuest"}},
                        {"changeDialog", {"h_changeDialog"}},
                        {"changeRoom", {"h_changeRoom"}} }}
                    };

    m_templates["standard"] = {
                    {"name", "standard"}, {"permeable",false}, {"help","standard.txt"},   
                    {"handlers",{
                        {"go", {"h_goTo"}},
                        {"look",{"h_look"}}, 
                        {"search",{"h_search"}}, 
                        {"talk",{"h_startDialog"}}, 
                        {"pick",{"h_take"}}, 
                        {"consume",{"h_consume"}}, 
                        {"read",{"h_read"}}, 
                        {"equipe",{"h_equipe"}}, 
                        {"dequipe",{"h_dequipe"}}, 
                        {"mode",{"h_changeMode"}}, 
                        {"printText", {"h_ignore"}},
                        {"show",{"h_ignore"}}, 
                        {"examine",{"h_ignore"}},
                        {"try", {"h_try"}}}},
                    };

    m_templates["fight"] = {
                    {"name","fight"}, {"permeable",false}, {"help","fight.txt"}, 
                    {"handlers",{
                        {"show",{"h_fight_show"}}}}
                    };

    m_templates["dialog"] = {{"name","dialog"}, {"permeable",false},{"help","dialog.txt"},{"error","Das kann ich beim besten Willen nicht sagen!"}};

    m_templates["trade"] = {
                    {"name","trade"}, {"permeable",false}, {"help","trade.txt"}, 
                    {"handlers",{
                        {"kaufe", {"h_buy"}},
                        {"verkaufe",{"h_sell"}},
                        {"zurück",{"h_exit"}}}}
                    };
    m_templates["chat"] = {
                    {"name", "chat"}, {"permeable", false}, {"help","chat.txt"},
                    {"handlers",{
                        {"[end]",{"h_end"}}}}
                    };

    m_templates["read"] = {
                    {"name","read"}, {"permeable",false}, {"help","read.txt"},
                    {"handlers",{
                        {"next", {"h_next"}},
                        {"previous", {"h_prev"}},
                        {"mark", {"h_mark"}},
                        {"underline", {"h_underline"}},
                        {"end",{"h_stop"}}}}
                    };

    m_templates["room"] = {{"name", "room"}, {"permeable", true,}, {"infos", {}}};
}

// ***** ***** FUNCTIONS ***** ****** // 


// *** add and delete time events *** //
void Context::add_timeEvent(std::string id, std::string scope, std::string info, double duration, int priority) {
    m_timeevents.insert(new CTimeEvent(id, scope, info, duration), priority, id);
}

bool Context::timeevent_exists(std::string type)
{
    if(m_timeevents.getContext(type) != NULL)
        return true;    
    return false;
}

void Context::deleteTimeEvent(std::string name)
{
    m_timeevents.erase(name);
}

// *** add and delete listeners *** //

void Context::add_listener(nlohmann::json j)
{
    if(j.count("regex") > 0)
        add_listener(j["id"], (std::regex)j["regex"], j.value("take", 1), j.value("priority", 0)); 
    else if(j.count("string") > 0)
        add_listener(j["id"], (std::string)j["string"], j.value("priority", 0));

}
void Context::add_listener(std::string sID, std::string sEventType, int priority) {
    m_eventmanager.insert(new CListener(sID, sEventType), priority, sID);
}

void Context::add_listener(std::string sID, std::regex eventType, int pos, int priority) {
    m_eventmanager.insert(new CListener(sID, eventType, pos), priority, sID);
}

void Context::add_listener(std::string sID, std::vector<std::string> eventType, int priority) {
    m_eventmanager.insert(new CListener(sID, eventType), priority, sID);
}

void Context::add_listener(std::string sID, std::map<std::string, std::string> eventType, int priority) {
    m_eventmanager.insert(new CListener(sID, eventType), priority, sID);
}

void Context::initializeHandlers(std::vector<nlohmann::json> 
    listeners) {
  for (auto it : listeners) {
    if (it.count("regex") > 0) {
      add_listener(it["id"], (std::regex)it["regex"], it.value("take", 1), 
          it.value("priority", 0)); 
    }
    else if (it.count("string") > 0)
      add_listener(it["id"], (std::string)it["string"], it.value("priority", 0));
  }
}


// *** Throw events *** //
bool Context::throw_event(event e, CPlayer* p) {
  m_curPermeable = m_permeable;
  m_block = false;
  bool called = false;
  m_curEvent = e;
  
  std::deque<CListener*> sortedEventmanager = m_eventmanager.getSortedCtxList();
  for (size_t i=0; i<sortedEventmanager.size() && m_block == false; i++) {
    event cur_event = m_curEvent;
    if (sortedEventmanager[i]->checkMatch(cur_event) == true) {
      if (m_handlers.count(sortedEventmanager[i]->getID()) > 0)
        (this->*m_handlers[sortedEventmanager[i]->getID()])(cur_event.second, p);
      else {
        p->printError("ERROR, given handler not found: " 
            + sortedEventmanager[i]->getID() + "\n");
      }
      called = true;
    }
  }   

  if (called == false)
    error(p);
      
  return m_curPermeable;
}

void Context::throw_timeEvents(CPlayer* p)
{
    //Check if player is currently occupied.
    if(p->getContexts().nonPermeableContextInList() == true)
        return;

    //Get current time.
    auto end = std::chrono::system_clock::now();

    //Check if a time event is ready to throw, then throw.
    std::deque<CTimeEvent*> sortedEvents = m_timeevents.getSortedCtxList();
    for(size_t i=0; i<sortedEvents.size(); i++)
    {
        std::chrono::duration<double> diff = end - sortedEvents[i]->getStart();
        if(diff.count() >= sortedEvents[i]->getDuration())
        {
            std::string str = sortedEvents[i]->getInfo();
            (this->*m_handlers[sortedEvents[i]->getID()])(str, p);
            std::cout << "BACKPASS: " << str << std::endl;
            if(str == "delete")
                m_timeevents.eraseHighest(sortedEvents[i]->getID());
        }
    }
}

// ***** ERROR FUNCTIONS ****** ***** //

void Context::error(CPlayer* p)
{
    if(m_permeable == false && m_sError != "")
        p->appendErrorPrint(m_sError);
    else if(m_permeable == false)
        p->appendTechPrint("Falsche Eingabe, gebe \"help\" ein, falls du nicht weiter weißt. (" +m_sName+")");
    else
        std::cout << "Context " << m_sName << " called with error state, probably normal.\n";
}

void Context::error_delete(CPlayer* p)
{
    std::cout << "Error from context " << m_sName << " called. Context will be deleted.\n";
    p->getContexts().erase(m_sName); 
}


// ***** ***** ***** HANDLER ***** ***** ***** //

void Context::h_help(std::string &sIdentifier, CPlayer* p)
{    
    if(m_sHelp != "")
    {
        std::ifstream read("factory/help/"+m_sHelp);

        std::string str((std::istreambuf_iterator<char>(read)),
                    std::istreambuf_iterator<char>());
        p->appendPrint("\n<b>Help: </b>\n" + str);
    }
    else if(m_permeable == false)
        p->appendTechPrint("No help for this context, sorry.\n");
}

// ***** ***** GAME CONTEXT ***** ***** //

void Context::h_reloadGame(std::string&, CPlayer* p)
{
    p->appendPrint("reloading game... \n");
    m_curPermeable = false;
}

void Context::h_reloadPlayer(std::string& sPlayer, CPlayer*p)
{
    p->appendPrint("reloading Player: " + sPlayer + "... \n");
    if(m_game->reloadPlayer(sPlayer) == false)
        p->appendPrint("Player does not exist... reloading world failed.\n");
    else
        p->appendPrint("Done.\n");
    m_curPermeable = false;
}

void Context::h_reloadWorlds(std::string& sPlayer, CPlayer*p)
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

void Context::h_finishCharacter(std::string& sIdentifier, CPlayer* p) 
{
    std::cout << "h_finishCharacter: " << sIdentifier << std::endl;

    //Get character
    CPerson* person = p->getWorld()->getCharacter(sIdentifier);

    if(person->getFaint() == false)
    {
        std::cout << "kill character\n";
        p->throw_events("killCharacter " + sIdentifier, "h_finishCharacter");
    }
    else
    {
        std::cout << "Character fainted.\n";
        if(person->getDialog("faint") != nullptr)
            person->setDialog("faint");
        else
            person->setDialog(p->getWorld()->getDialog("faintDialog"));
    } 
    m_curPermeable=false;
}
void Context::h_killCharacter(std::string& sIdentifier, CPlayer* p) {

    //Get character
    CPerson* person = p->getWorld()->getCharacter(sIdentifier);

    //Create json for details
    nlohmann::json jDetail = {
                {"id", person->getID()},  
                {"name", person->getName()}, 
                {"look", ""}};

    //Parse details-description
    if(person->getDeadDescription().size() == 0)
    {
        nlohmann::json j = { {"speaker", "PERZEPTION"}, {"text", "Ein totes etwas?"} };
        jDetail["description"].push_back(j);
    }
    else
        jDetail["description"] = person->getDeadDescription();

    //Create new detail and add to room and map of details
    CDetail* detail = new CDetail(jDetail, p, person->getInventory().mapItems());
    p->getRoom()->getDetails()[person->getID()] = detail;
    h_deleteCharacter(sIdentifier, p);
}
void Context::h_deleteCharacter(std::string& sIdentifier, CPlayer* p) {
    p->getRoom()->getCharacters().erase(sIdentifier);
    delete p->getWorld()->getCharacter(sIdentifier);
    p->getWorld()->getCharacters().erase(sIdentifier); 

    m_curPermeable=false;
}

void Context::h_addItem(std::string& sIdentifier, CPlayer* p) {
    p->addItem(p->getWorld()->getItem(sIdentifier, p));
    m_curPermeable=false;
}

void Context::h_recieveMoney(std::string& sIdentifier, CPlayer* p) {
    p->setStat("gold", p->getStat("gold") + stoi(sIdentifier));
    p->appendPrint(Webcmd::set_color(Webcmd::color::GREEN) + "+" + sIdentifier + " Schiling" + Webcmd::set_color(Webcmd::color::WHITE) + "\n");

   m_curPermeable = false; 
}

void Context::h_eraseMoney(std::string& sIdentifier, CPlayer* p) {
    p->setStat("gold", p->getStat("gold") - stoi(sIdentifier));
    p->appendPrint(Webcmd::set_color(Webcmd::color::RED) + "-" + sIdentifier + " Schiling" + Webcmd::set_color(Webcmd::color::WHITE) + "\n");

   m_curPermeable = false; 
}
void Context::h_endFight(std::string& sIdentifier, CPlayer* p) {
    p->endFight();
    m_curPermeable=false;
}

void Context::h_endDialog(std::string& sIdentifier, CPlayer* p) {
    p->getContexts().erase("dialog");
    m_curPermeable=false;
}

void Context::h_newFight(std::string& sIdentifier, CPlayer* p) {

    std::cout << "h_newFight: " << sIdentifier << std::endl;
    auto lambda = [](CPerson* person){return person->getName(); };
    std::string str =func::getObjectId(p->getRoom()->getCharacters(),sIdentifier,lambda);
   
    if(p->getWorld()->getCharacter(str) != nullptr)
        p->setFight(new CFight(p, p->getWorld()->getCharacter(str)));

    m_curPermeable=false;
}

void Context::h_gameover(std::string& sIdentifier, CPlayer* p) {
    p->setStat("hp", 0);
    p->appendStoryPrint("\nDu bist gestorben... \n\n $\n");
}

void Context::h_addQuest(std::string& sIdentifier, CPlayer* p) {
    p->setNewQuest(sIdentifier);
    m_curPermeable=false;
}

void Context::h_showPersonInfo(std::string& sIdentifier, CPlayer* p) {
    auto lambda = [](CPerson* person) { return person->getName();};
    std::string character = func::getObjectId(p->getRoom()->getCharacters(), sIdentifier, lambda);

    if(character != "")
        p->appendPrint(p->getWorld()->getCharacter(character)->getAllInformation());
    else
        p->appendPrint("character not found.\n");
    m_curPermeable=false;
}
void Context::h_showItemInfo(std::string& sIdentifier, CPlayer* p) 
{
    auto lambda = [](CItem* item) { return item->getName(); };
    std::string item = func::getObjectId(p->getRoom()->getItems(), sIdentifier, lambda);

    if(item != "")
        p->appendPrint(p->getRoom()->getItems()[item]->getAllInfos());
    else
        p->appendPrint("Item not found.\n");
    m_curPermeable=false;
}
void Context::h_changeName(std::string& sIdentifier, CPlayer* p)
{
    std::vector<std::string> v_atts = func::split(sIdentifier, "|");
    if(v_atts[0] == "character")
    {
        if(p->getWorld()->getCharacter(v_atts[1]) != NULL)
        {
            p->getWorld()->getCharacter(v_atts[1])->setName(v_atts[2]);
            std::cout << "Name changed to: " << p->getWorld()->getCharacter(v_atts[1])->getName() << std::endl;
        }
    }
    m_curPermeable=false;
}

void Context::h_setAttribute(std::string& sIdentifier, CPlayer* p)
{
    //Get vector with [0]=attribute to modify, [1]=operand, [2]=value
    std::vector<std::string> atts = func::split(sIdentifier, "|");

    //Check if sIdentifier contains the fitting values
    if(atts.size() != 3 || std::isdigit(atts[2][0]) == false || p->getStat(atts[0]) == 999) 
        std::cout << "Something went worng! Player Attribute could not be changed.\n";

    //Modify attribute according to operand.
    else if(atts[1] == "=")
        p->setStat(atts[0], stoi(atts[2]));
    else if(atts[1] == "+")
        p->setStat(atts[0], p->getStat(atts[0]) + stoi(atts[2]));
    else if(atts[1] == "-")
        p->setStat(atts[0], p->getStat(atts[0]) - stoi(atts[2]));
    else
        std::cout << "Wrong operand for setting attribute." << "\n";

    m_curPermeable=false;
}

void Context::h_setNewAttribute(std::string& sIdentifier, CPlayer* p)
{
    std::vector<std::string> atts = func::split(sIdentifier, "|");
    
    //Check if sIdentifier contains the fitting values
    if(atts.size() != 2 || std::isdigit(atts[1][0]) == false)
        std::cout << "Something went worng! Player Attribute could not be changed.\n";
    else
        p->getStats()[atts[0]] = stoi(atts[1]);
    m_curPermeable=false;
}

void Context::h_addTimeEvent(std::string& sIdentifier, CPlayer* p)
{
    std::vector<std::string> atts = func::split(sIdentifier, ",");

    //Check if sIdentifier contains the fitting values
    if(atts.size()!=4 || std::isdigit(atts[3][0])==false || std::isdigit(atts[2][0])==false)
        std::cout << "Something went worng! Time events could not be added.\n";
    else
    {
        std::cout << "Added to context: " << atts[1] << std::endl;
        p->getContext(atts[1])->add_timeEvent("t_throwEvent", atts[1], atts[0], std::stod(atts[3], nullptr), stoi(atts[2]));
    }

    m_curPermeable = false;
}

void Context::h_setNewQuest(std::string& sIdentifier, CPlayer* p)
{
    p->setNewQuest(sIdentifier);
    m_curPermeable = false;
}

void Context::h_changeDialog(std::string& sIdentifier, CPlayer* p)
{
    std::vector<std::string> atts = func::split(sIdentifier, "|");
    if(atts.size() != 2)
    {
        std::cout << "Something went worng! Dialog could not be changed.\n";
        return;
    }

    auto lambda = [](CPerson* person) { return person->getName(); };
    std::string str = func::getObjectId(p->getRoom()->getCharacters(), atts[0], lambda);
    p->getWorld()->getCharacter(str)->setDialog(atts[1]);
    
    m_curPermeable = false;
}

void Context::h_printText(std::string &sIdentifier, CPlayer* p) {
  if(p->getWorld()->getTexts().count(sIdentifier) > 0) {
    CText text(p->getWorld()->getTexts()[sIdentifier], p);
    p->appendPrint(text.print());
  }
  else
    std::cout << "Text not found!\n";
}

void Context::h_changeRoom(std::string& sIdentifier, CPlayer* p)
{
    CRoom* room = p->getWorld()->getRoom(sIdentifier);
    if(room != nullptr)
        p->setRoom(p->getWorld()->getRoom(sIdentifier));
    m_curPermeable = false;
}

// ***** ***** STANDARD CONTEXT ***** ***** //

void Context::h_ignore(std::string&, CPlayer*) {}

void Context::h_showExits(std::string& sIdentifier, CPlayer* p) {
    p->appendDescPrint(p->getRoom()->showExits(p->getMode(), p->getGramma())+"\n");
        p->addSelectContest(p->getRoom()->getExtits2(), "go");
}


void Context::h_show(std::string& sIdentifier, CPlayer* p) {
    if(sIdentifier == "exits" || sIdentifier == "ausgänge")
    {
        p->appendDescPrint(p->getRoom()->showExits(p->getMode(), p->getGramma())+"\n");
        p->addSelectContest(p->getRoom()->getExtits2(), "go");
    }
    else if(sIdentifier == "visited" || sIdentifier == "besuchte räume")
        p->showVisited();
    else if(sIdentifier == "people" || sIdentifier == "personen")
    {
        p->appendDescPrint(p->getRoom()->showCharacters(p->getMode(), p->getGramma()) + "\n"); 
        auto lambda = [](CPerson* person) {return person->getName();};
        p->addSelectContest(func::convertToObjectmap(p->getRoom()->getCharacters(), lambda) , "talk");
    }
    else if(sIdentifier == "room")
        p->appendPrint(p->getRoom()->showDescription(p->getWorld()->getCharacters()));
    else if(sIdentifier == "items" || sIdentifier == "gegenstände")
        p->appendDescPrint(p->getRoom()->showItems(p->getMode(), p->getGramma()) + "\n");
    else if(sIdentifier == "details" || sIdentifier == "mobiliar")
        p->appendDescPrint(p->getRoom()->showDetails(p->getMode(), p->getGramma()) + "\n");
    else if(sIdentifier == "inventory" || sIdentifier == "inventar")
        p->appendPrint(p->getInventory().printInventory());
    else if(sIdentifier == "equiped" || sIdentifier == "ausrüstung")
        p->printEquiped();
    else if(sIdentifier == "quests")
        p->showQuests(false);
    else if(sIdentifier == "solved quests" || sIdentifier == "gelöste quests")
        p->showQuests(true);
    else if(sIdentifier == "stats")
        p->showStats();
    else if(sIdentifier == "mind")
        p->showMinds();
    else if(sIdentifier == "attacks" || sIdentifier == "attacken")
        p->appendPrint(p->printAttacks());
    else if(sIdentifier == "all" || sIdentifier == "alles")
        p->appendDescPrint(p->getRoom()->showAll(p->getMode(), p->getGramma()));
    else
        p->appendErrorPrint("Unbekannte \"zeige-function\"\n"); 
}

void Context::h_look(std::string& sIdentifier, CPlayer* p) 
{
    //Extract details (sWhat) and where to look (sWhere) from identifier
    size_t pos = sIdentifier.find(" ");
    if(pos == std::string::npos)
    {
        p->appendErrorPrint("Ich weiß nicht, was du durchsuchen willst.\n");
        return;
    }
    
    std::string sWhere = sIdentifier.substr(0, pos);
    std::string sWhat = sIdentifier.substr(pos+1);
    auto lambda = [](CDetail* detail) { return detail->getName();};
    std::string sDetail = func::getObjectId(p->getRoom()->getDetails(), sWhat, lambda);

    //Check whether input is correct/ detail could be found.
    if(sDetail == "")
    {
        p->appendErrorPrint("Ich weiß nicht, was du durchsuchen willst.\n");
        return;
    }

    //Check whether sWhere matched with detail
    CDetail* detail  = p->getRoom()->getDetails()[sDetail];
    if(detail->getLook() == sWhere)
        p->appendDescPrint(p->getRoom()->look(sDetail, p->getMode(), p->getGramma()));
    else
        p->appendErrorPrint("Ich kann nicht " + sWhere + " " + detail->getName() + " schauen.\nSoll ich in, auf oder unter " + detail->getName() + " schauen?\n");
}

void Context::h_search(std::string& sIdentifier, CPlayer* p)
{
    auto lambda = [](CDetail* detail) { return detail->getName();};
    std::string sDetail = func::getObjectId(p->getRoom()->getDetails(), sIdentifier, lambda);

    //Check whether input is correct/ detail could be found.
    if(sDetail == "")
    {
        p->appendErrorPrint("Ich weiß nicht, was du durchsuchen willst.\n");
        return;
    }
    
    CDetail* detail  = p->getRoom()->getDetails()[sDetail];

    //Check whether location is neccessary
    if(detail->getLook() == "")
        p->appendDescPrint(p->getRoom()->look(sDetail, p->getMode(), p->getGramma()));
    else
        p->appendDescPrint("Ich weiß nicht, wo ich suchen soll. Sag: \"schaue [in/ unter/ ...] [gegenstand]\"\n");
}

void Context::h_goTo(std::string& sIdentifier, CPlayer* p) {
    p->changeRoom(sIdentifier);
}

void Context::h_startDialog(std::string& sIdentifier, CPlayer* p)
{
    //Get selected character
    auto lambda1 = [](CPerson* person) { return person->getName(); };
    std::string character = func::getObjectId(p->getRoom()->getCharacters(), sIdentifier, lambda1);
    auto lambda2 = [](CPlayer* player) { return player->getName(); };
    std::string player = func::getObjectId(p->getMapOFOnlinePlayers(), sIdentifier, lambda2);

    //Check if character was found
    if(character != "") 
        p->startDialog(character);  
    else if(player != "") 
        p->startChat(p->getPlayer(player));
    else
        p->appendErrorPrint("Character not found");
}

void Context::h_take(std::string& sIdentifier, CPlayer* p) {
    if(sIdentifier.find("all") == 0)
        p->addAll();
    else if(p->getRoom()->getItem(sIdentifier) == NULL)
    {
        std::cout << "Not found.\n";
        p->appendErrorPrint("Item not found.\n");
    }
    else
    {
        std::cout << "Getting item.\n";
        p->addItem(p->getRoom()->getItem(sIdentifier));
    }
}

void Context::h_consume(std::string& sIdentifier, CPlayer* p) {
    if(p->getInventory().getItem(sIdentifier) != NULL) {
        if(p->getInventory().getItem(sIdentifier)->callFunction(p) == false)
            p->appendTechPrint("Dieser Gegenstand kann nicht konsumiert werden.\n");
    }
    else
        p->appendErrorPrint("Gegenstand nicht in deinem Inventar! (benutze \"zeige Inventar\" um deine Items zu sehen.)\n");
}

void Context::h_read(std::string& sIdentifier, CPlayer* p) {
    if(p->getInventory().getItem(sIdentifier) != NULL) {
        if(p->getInventory().getItem(sIdentifier)->callFunction(p) == false)
            p->appendTechPrint("Dieser Gegenstand kann nicht gelesen werden.\n");
    }
    else
        p->appendErrorPrint("Gegenstand nicht in deinem Inventar! (benutze \"zeige Inventar\" um alle deine Gegenstande zu sehen.)\n");
}

void Context::h_equipe(std::string& sIdentifier, CPlayer* p) {
    if(p->getInventory().getItem(sIdentifier) != NULL) {
        if(p->getInventory().getItem(sIdentifier)->callFunction(p) == false)
            p->appendErrorPrint("Dieser Gegenstand kann nicht ausgerüstet werden.\n");
    }
    else
        p->appendErrorPrint("Gegenstand nicht in deinem Inventar! (benutze \"zeige Inventar\" um deine Items zu sehen.)\n");
}

void Context::h_dequipe(std::string& sIdentifier, CPlayer* p) {
    p->dequipeItem(sIdentifier);
}

void Context::h_examine(std::string &sIdentifier, CPlayer*p) {
    //Check for room 
    if(sIdentifier == "raum")
    {
        p->appendPrint(p->getRoom()->showDescription(p->getWorld()->getCharacters()));
        return;
    }

    //Check for detail
    auto lambda1 = [](CDetail* detail) { return detail->getName(); };
    std::string sObject = func::getObjectId(p->getRoom()->getDetails(), sIdentifier, lambda1);
    if(sObject != "")
        p->appendPrint(p->getRoom()->getDetails()[sObject]->getDescription());

    auto lambda2 = [](CItem* item) { return item->getName(); };
    sObject = func::getObjectId(p->getRoom()->getItems(), sIdentifier, lambda2);
    //Check for item
    if(sObject != "")
        p->appendPrint(p->getRoom()->getItems()[sObject]->getDescription());

    //Check for person
    auto lambda3 = [](CPerson* person) { return person->getName();};
    sObject = func::getObjectId(p->getRoom()->getCharacters(), sIdentifier, lambda3);
    //Check for item
    if(sObject != "")
        p->appendPrint(p->getWorld()->getCharacter(sObject)->getDescription());
}

void Context::h_changeMode(std::string& sIdentifier, CPlayer* p) {
    p->changeMode();
}

void Context::h_test(std::string& sIdentifier, CPlayer* p) {
    p->appendPrint(sIdentifier);
}


// **** SPECIAL HANDLER ***** //

void Context::h_moveToHospital(std::string& sIdentifier, CPlayer* p)
{
    //Get selected room
    auto lambda = [](CExit* exit) { return exit->getPrep() + "_" + exit->getName();};
    if(p->getRoom()->getID().find("zug_compartment") == std::string::npos || func::getObjectId(p->getRoom()->getExtits(), sIdentifier, lambda) != "zug_trainCorridor")
        return;

    p->changeRoom(p->getWorld()->getRoom("hospital_foyer"));
    m_block = true;
    m_curPermeable=false;
}

void Context::h_exitTrainstation(std::string& sIdentifier, CPlayer* p)
{
    std::cout << "h_exitTrainstation, " << sIdentifier << std::endl;

    auto lambda= [](CExit* exit) { return exit->getPrep() + " " + exit->getName(); };
    if(p->getRoom()->getID() != "trainstation_eingangshalle" || func::getObjectId(p->getRoom()->getExtits(), sIdentifier, lambda) != "trainstation_ausgang")
        return;

    p->appendStoryPrint("Du drehst dich zum dem großen, offen stehenden Eingangstor der Bahnhofshalle. Und kurz kommt dir der Gedanke doch den Zug nicht zu nehmen, doch alles beim Alten zu belassen. Doch etwas sagt dir, dass es einen guten Grund gab das nicht zu tun, einen guten Grund nach Moskau zu fahren. Und auch, wenn du ihn gerade nicht mehr erkennst. Vielleicht ist gerade das der beste Grund: rausfinden, was dich dazu getrieben hat, diesen termin in Moskau zu vereinbaren.\n Du guckst dich wieder in der Halle um, und überlegst, wo du anfängst zu suchen.\n");
    
    m_block=true;
}

void Context::h_thieve(std::string& sIdentifier, CPlayer* p)
{
    if(m_jAttributes["infos"].count("h_thieve") > 0)
    {
        std::string str = m_jAttributes["infos"]["h_thieve"];
        p->startDialog(str, p->getWorld()->getCharacter(str)->getDialog("thieve"));
    }

    else
        p->appendStoryPrint("You know, me son. You should not be stealing!");
    m_curPermeable = false;
}

void Context::h_attack(std::string& sIdentifier, CPlayer* p)
{
    std::cout << "h_attack: " << sIdentifier << std::endl;

    if(m_jAttributes["infos"].count("h_attack") == 0)
        return; 

    std::string character = m_jAttributes["infos"]["h_attack"];
    std::cout << "Infos: " << character << std::endl;
    p->setFight(new CFight(p, p->getWorld()->getCharacter(character)));
    m_curPermeable = false;
}


void Context::h_try(std::string& sIdentifier, CPlayer* p)
{
    p->throw_events("zeige details", "try");
    p->throw_events("zeige personen", "try");
    p->throw_events("zeige ausgänge", "try");
    p->throw_events("go neben", "try");
    p->throw_events("go Toil", "try");
    p->throw_events("go frauen", "try");
    p->throw_events("2", "try");

}


// ***** ***** FIGHT CONTEXT ***** ***** //
void Context::initializeFightListeners(std::map<std::string, std::string> mapAttacks)
{
    add_listener("h_fight", mapAttacks);
}

void Context::h_fight(std::string& sIdentifier, CPlayer* p) {
    std::string newCommand = p->getFight()->fightRound((sIdentifier));
    if(newCommand != "")    
        p->throw_events(newCommand, "h_fight");
}

void Context::h_fight_show(std::string& sIdentifier, CPlayer* p) {
    if(sIdentifier == "stats")
        p->appendPrint(p->getFight()->printStats(p));
    else if(sIdentifier == "opponent stats")
        p->appendPrint(p->getFight()->printStats(p->getFight()->getOpponent()));
    else
        p->appendPrint("Falsche Eingabe! Versuche es mit \"show stats\" oder \"show opponent stats\"\n");
}


// ***** ***** DIALOG CONTEXT ***** *****

void Context::initializeDialogListeners(std::string newState, CPlayer* p)
{
    //Set (new) state.
    setAttribute<std::string>("state", newState); 

    //Clear listeners.
    m_eventmanager.clear();

    //Add help listener 
    add_listener("h_help", "help");

    //Add listener for each dialog option.
    std::vector<size_t> activeOptions = p->getDialog()->getState(newState)->getActiveOptions(p);
    std::map<size_t, size_t> mapOtptions;
    size_t counter = 1;
    for(auto opt : activeOptions)
    {
        mapOtptions[counter] = opt;
        add_listener("h_call", std::to_string(counter));
        counter++;
    }
    setAttribute<std::map<size_t, size_t>>("mapOptions", mapOtptions);
}

void Context::h_call(std::string& sIdentifier, CPlayer* p)
{
    size_t option = getAttribute<std::map<size_t, size_t>>("mapOptions")[stoi(sIdentifier)];

    std::string curState = getAttribute<std::string>("state");
    std::string nextState = p->getDialog()->getState(curState)->getNextState(std::to_string(option), p);

    if(nextState == "trade")
        p->startTrade(getAttribute<std::string>("partner"));
    else
    {
        initializeDialogListeners(nextState, p);
        std::string newCommand = p->getDialog()->getState(nextState)->callState(p);
        if(newCommand != "")
            p->throw_events(newCommand, "h_call");
    }
}

// ***** ***** TRADE CONTEXT ***** ***** // 

void Context::print(CPlayer* p)
{
    CPerson* partner = p->getWorld()->getCharacter(getAttribute<std::string>("partner"));
    p->appendPrint("<b>" + p->getName() + "s Inventar:</b>\n" 
                    + p->getInventory().printInventory()
                    + "\n<b>" + partner->getName() + "s Inventar: </b>\n" 
                    + partner->getInventory().printInventory());
}

void Context::h_sell(std::string& sIdentifier, CPlayer* p)
{
    CPerson* partner = p->getWorld()->getCharacter(getAttribute<std::string>("partner"));
    CItem* curItem = p->getInventory().getItem(sIdentifier);
    if(curItem == NULL)
        p->appendErrorPrint("Dieser Gegenstand befindet sich nicht in deinem Inventar, du kannst ihn logischerwiese dehalb nicht verkaufen!\n");
    else if(p->getEquipment().count(curItem->getType()) > 0 && p->getEquipment()[curItem->getType()]->getID() == curItem->getID())
        p->appendErrorPrint("Du kannst ausgerüstete Gegenstände nicht verkaufen.\n");
    else
    {
        p->appendDescPrint("Du hast " + curItem->getName() + " verkauft.\n\n");
        p->throw_events("recieveMoney " + std::to_string(curItem->getValue()), "h_sell");
        CItem* item = new CItem(curItem->getAttributes(), p);
        partner->getInventory().addItem(item);
        p->getInventory().removeItemByID(curItem->getID());
    }
    print(p);
}

void Context::h_buy(std::string& sIdentifier, CPlayer* p)
{
    CPerson* partner = p->getWorld()->getCharacter(getAttribute<std::string>("partner"));
    CItem* curItem = partner->getInventory().getItem(sIdentifier);

    if(curItem == NULL)
        p->appendErrorPrint("Dieser Gegenstand befindet sich nicht in dem Inventar des Händlers, du kannst ihn logischerwiese dehalb nicht kaufen!\n");
    else
    {
        if(p->getStat("gold") < curItem->getValue())
        {
            p->appendErrorPrint("Du hast nicht genuegend Geld um "+curItem->getName()+" zu kaufen");
            return;
        }

        p->appendDescPrint("Du hast " + curItem->getName() + " gekauft.\n\n");
        p->setStat("gold", p->getStat("gold") - curItem->getValue());
        p->getInventory().addItem(new CItem(curItem->getAttributes(), p));
        partner->getInventory().removeItemByID(curItem->getID());
    }
    print(p);
}

void Context::h_exit(std::string&, CPlayer* p)
{
    CPerson* partner = p->getWorld()->getCharacter(getAttribute<std::string>("partner"));
    p->getContexts().erase("trade");
    p->getContexts().erase("dialog");
    p->startDialog(partner->getID());
}

// ***** ***** CHAT CONTEXT ***** ***** //
void Context::h_send(string& sInput, CPlayer* p)
{
    CPlayer* chatPartner = p->getPlayer(getAttribute<std::string>("partner"));
    chatPartner->send(p->returnSpeakerPrint(func::returnToUpper(p->getName()), sInput + "\n"));
    p->appendSpeackerPrint("YOU", sInput + "\n");
}

void Context::h_end(string& sMessage, CPlayer* p)
{
    CPlayer* chatPartner = p->getPlayer(getAttribute<std::string>("partner"));
    if(sMessage == "[end]")
        chatPartner->send("[Gespräch beendet].\n");
    else
    {
        chatPartner->send(p->returnSpeakerPrint(func::returnToUpper(p->getName()), sMessage));
        chatPartner->send("[Gespräch beendet].\n");
    }

    chatPartner->getContexts().erase("chat");

    p->appendPrint("Gespräch mit " + chatPartner->getName() + " beendet.\n");
    p->getContexts().erase("chat");
    m_block = true; 
}


// ***** ***** CHAT CONTEXT ***** ***** //

void Context::h_next(std::string&, CPlayer* p)
{
    std::cout << "h_next\n";
    std::cout << "Item: " << m_jAttributes["item"] << std::endl;
    CItem* item = p->getInventory().getItem_byID(m_jAttributes["item"]);

    std::cout << "Mark: " << m_jAttributes["mark"] << std::endl;
    std::cout << "Pages.size(): " << item->getPages()->getNumPages() << std::endl;
    if(m_jAttributes["mark"] == item->getPages()->getNumPages()-1)
        p->appendDescPrint("Das Buch ist zuende.");
    else
    {
        m_jAttributes["mark"] = m_jAttributes["mark"].get<size_t>()+1;
        p->appendPrint(
              "Seite: " + std::to_string(m_jAttributes["mark"].get<int>()+1) + ": \n"
            + item->getPages()->pagePrint(m_jAttributes["mark"]) + "\n");
    }
}

void Context::h_prev(std::string&, CPlayer* p)
{
    std::cout << "h_prev\n";
    std::cout << "Item: " << m_jAttributes["item"] << std::endl;
    CItem* item = p->getInventory().getItem_byID(m_jAttributes["item"]);
    if(m_jAttributes["mark"] == 0)
        p->appendDescPrint("Das Buch ist bereits auf der ersten Seite aufgeschlagen.");
    else
    {
        m_jAttributes["mark"] = m_jAttributes["mark"].get<size_t>() -1;
        p->appendPrint(
              "Seite: " + std::to_string(m_jAttributes["mark"].get<int>()+1) + ": \n"
            + item->getPages()->pagePrint(m_jAttributes["mark"]) + "\n");
    }
}

void Context::h_mark(std::string&, CPlayer* p)
{
    std::cout << "h_mark\n";
    std::cout << "Item: " << m_jAttributes["item"] << std::endl;

    p->getInventory().getItem_byID(m_jAttributes["item"])->setMark(m_jAttributes["mark"]);
    size_t mark = m_jAttributes["mark"];
    p->appendPrint("Leesezeichen auf Seite " + std::to_string(mark+1) + " gesetzt.");
}

void Context::h_underline(std::string& sIdentifier, CPlayer* p)
{
    if(sIdentifier.find(",") == std::string::npos)
    {
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

void Context::h_stop(std::string&, CPlayer* p)
{
    p->appendDescPrint("Ich habe das Buch wieder zugeschlagen.\n");
    p->getContexts().erase("read"); 
}


// ***** ***** QUEST CONTEXT ***** ***** //

void Context::h_react(std::string& sIdentifier, CPlayer* p) {
  std::cout << "h_react: " << sIdentifier << std::endl;
  std::cout << "Quest: " << getAttribute<std::string>("questID") << std::endl;

  //Obtain quest and quest-step
  CQuest* quest = p->getWorld()->getQuest(getAttribute<std::string>("questID"));

  for (auto it : quest->getSteps()) {
    std::cout << it.first << std::endl;
    

    LogicParser logic({{"room", p->getRoom()->getID()}, {"inventory", 
        p->getInventory().getItemList()}, {"cmd", m_curEvent.first}, 
        {"input", sIdentifier}});
    if (logic.Success(it.second->logic()) == true 
        && p->checkDependencies(it.second->getDependencies()) == true) {
      p->questSolved(quest->getID(), it.first);
    }

    /**
     * old check function.
    //If quest infos don't block, solve quest.
    if (CheckQuestStep(it.second, sIdentifier, p) == true)
      p->questSolved(quest->getID(), it.first);
    */
  }
}

bool Context::CheckQuestStep(CQuestStep* step, std::string sIdentifier,
    CPlayer* p) {
  //If quest is already solved, skip.
  if (step->getSolved() == true)
    return false;

  // --- Check if player-situation/ -input matches with quest-step --- //
  std::map<std::string, std::string> infos = step->getInfo();
  
  //Check if function simply passes, if yes, compare with player-command ("pass"). 
  if (infos.count("pass") > 0 && infos["pass"] == m_curEvent.first)
    return true; 

  //Check for items in inventory ("inventory").
  if (infos.count("inventory")>0) {
    bool nor = false;
    for (auto it : func::split(infos["inventory"], "|")) {
      if (p->getInventory().getItem_byID(it))
        nor = true;
    }
    if (nor == false) return false; 

    for (auto it : func::split(infos["inventory"], "&")) {
      if (!p->getInventory().getItem_byID(it))
        return false;
    }
  }

  //Check for current room ("location").
  if (infos.count("location") > 0) {
    bool nor = false;
    for (auto it : func::split(infos["location"], "|")) {
      if (p->getInventory().getItem_byID(it))
        nor = true;
    }
    if (nor == false) return false; 
  }

  //Check for dependencies 
  if (p->checkDependencies(step->getDependencies()) == false)
    return false;

  //(fuzzy-)Compare identifier ("string"/ "fuzzy").
  if ((infos.count("string") > 0 && infos["string"] == sIdentifier) 
      || (infos.count("fuzzy") > 0 && fuzzy::fuzzy_cmp(infos["fuzzy"], 
          sIdentifier) <= 0.2))
    return true;
  
  //Check if function simply passes, if yes, compare with player-command ("pass"). 
  if (infos.count("pass") > 0 && infos["pass"] == "anything")
    return true; 

  return false;
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
void Context::h_reden(std::string& sIdentifier, CPlayer* p)
{
    auto lambda = [](CPerson* person) { return person->getName();};
    std::string character = func::getObjectId(p->getRoom()->getCharacters(), sIdentifier, lambda);
    if(character == "" || character.find("passanten_gruppe") == std::string::npos)
        return;

    CQuest* quest = p->getWorld()->getQuest(getAttribute<std::string>("questID"));
    CQuestStep* step = quest->getSteps()["1reden"];
    
    //If character ID ends with a number, delete.
    if(std::isdigit(character.back()) == true)
        character.pop_back();

    for(size_t i=0; i<step->getWhich().size(); i++) {
        if(step->getWhich()[i].find(character) != std::string::npos)
            return;
    }

    step->getWhich().push_back(character);
    step->incSucc(1);
    p->questSolved(quest->getID(), "1reden");

    //Change dialog of all "Passanten"
    if(step->getSolved() == true)
    {
        std::cout << "CHANGING DIALOGS OF PASSANTEN\n";
        p->getWorld()->getCharacter("trainstation_eingangshalle_passanten_gruppe")->setDialog("2");
        p->getWorld()->getCharacter("trainstation_nebenhalle_passanten_gruppe")->setDialog("2");
        p->getWorld()->getCharacter("trainstation_gleis5_passanten_gruppe")->setDialog("2");
        p->getContexts().erase(quest->getID());
    }
}

// *** *** GELD AUFTREIBEN *** *** //
void Context::h_geldauftreiben(std::string& sIdentifier, CPlayer* p)
{
    if(p->getStat("gold") + stoi(sIdentifier) < 10)
        return;

    CQuest* quest = p->getWorld()->getQuest(getAttribute<std::string>("questID"));
    if(quest->getActive() == true)
        p->appendDescPrint("Wundervoll, genug Geld, um das Ticket zu kaufen!\n");

    p->questSolved(quest->getID(), "1geldauftreiben");
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

void Context::h_choose_equipe(std::string& sIdentifier, CPlayer* p)
{
    if(sIdentifier == "yes") {
        p->dequipeItem("weapon");
        p->equipeItem(p->getInventory().getItem_byID(getAttribute<std::string>("itemID")), "weapon");
        p->getContexts().erase("equipe");
    }

    else if(sIdentifier == "no") {
        p->appendDescPrint("Du entscheidest dich " + p->getInventory().getItem_byID(getAttribute<std::string>("itemID"))->getName() + " nicht auszurüsten.\n");
        p->getContexts().erase("equipe");
    }

    else
        error(p);
}

void Context::h_updateStats(std::string& sIdentifier, CPlayer* p)
{
    //Get ability (by number or by name/ id).
    std::string ability="";
    for(size_t i=0; i<p->getAbbilities().size(); i++)
    {
        if(fuzzy::fuzzy_cmp(func::returnToLower(p->getAbbilities()[i]), sIdentifier) <= 0.2)
            ability = p->getAbbilities()[i];
        if(std::isdigit(sIdentifier[0]) && i == stoul(sIdentifier, nullptr, 0)-1)
            ability = p->getAbbilities()[i];
    }

    //Update ability.
    p->setStat(ability, p->getStat(ability)+1);
    p->appendSuccPrint(ability + " updated by 1\n");
    int num = getAttribute<int>("numPoints")-1;
    p->getContexts().erase("updateStats");
    if(num>0)
        p->updateStats(num);
}


// ----- ***** TIME EVENTS ***** ------ //
void Context::t_highness(std::string& str, CPlayer* p)
{
    if(p->getStat("highness")==0)
        return;
    p->appendStoryPrint("Time always comes to give you a hand; Things begin to become clearer again. Highness decreased by 1.\n");
    p->setStat("highness", p->getStat("highness")-1);

    p->getContext("standard")->deleteTimeEvent("t_highness");
    if(p->getStat("highness") > 0)
        add_timeEvent("t_highness", "standard", "", 0.2);
}

void Context::t_throwEvent(std::string& sInfo, CPlayer* p) 
{
    std::cout << "t_throwEvent: " << sInfo << std::endl;
    std::string str = sInfo;
    p->addPostEvent(str);
    sInfo = "delete";
}
