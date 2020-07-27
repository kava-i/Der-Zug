#include "CEnhancedContext.hpp"
#include "CPlayer.hpp"
#include "CGame.hpp"

// ***** ***** CONSTRUCTORS ***** ***** //

std::map<std::string, void (CEnhancedContext::*)(std::string&, CPlayer* p)> CEnhancedContext::m_handlers = {};
std::map<std::string, nlohmann::json> CEnhancedContext::m_templates = {};

CEnhancedContext::CEnhancedContext(nlohmann::json jAttributes)
{
    m_jAttributes = jAttributes;
    m_sName = jAttributes.value("name", "context");
    m_permeable = jAttributes.value("permeable", false);
    m_curPermeable = m_permeable;
    m_block = false;
    m_sHelp = jAttributes.value("help", "");
    m_sError = jAttributes.value("error", "");

    if(jAttributes.count("handlers") > 0)
    {
        for(const auto &it : jAttributes["handlers"].get<std::map<string, vector<string>>>()) {
            for(size_t i=0; i<it.second.size(); i++)
                add_listener(it.second[i], it.first, i+1);
        }
    }
    add_listener("h_help", "help", 0);

    m_error = &CEnhancedContext::error;
}
 
CEnhancedContext::CEnhancedContext(std::string sTemplate) : CEnhancedContext(getTemplate(sTemplate))
{
}

CEnhancedContext::CEnhancedContext(std::string sTemplate, nlohmann::json jAttributes) : CEnhancedContext(getTemplate(sTemplate))
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
                add_listener(it.second[i], it.first, i+1);
        }
    }

    m_error = &CEnhancedContext::error;
}

// ***** ***** GETTER ***** ***** //
std::string CEnhancedContext::getName() {
    return m_sName;
}

bool CEnhancedContext::getPermeable() {
    return m_permeable;
}

nlohmann::json CEnhancedContext::getTemplate(std::string sTemplate)
{
    if(m_templates.count(sTemplate) == 0)
    {
        std::cout << "Template requested that does not exist: " << sTemplate << std::endl;
        return nlohmann::json::object();
    }
    return m_templates[sTemplate];
} 



// ***** ***** SETTER ***** ***** //
void CEnhancedContext::setName(std::string sName) {
    m_sName = sName;
}

void CEnhancedContext::setGame(CGame* game) {
    m_game = game;
}

void CEnhancedContext::setErrorFunction(void(CEnhancedContext::*func)(CPlayer* p)) {
    m_error = func;
}

// ***** ***** INITIALIZERS ***** ***** //
void CEnhancedContext::initializeHanlders()
{
    // ***** GENERAL STUFF ***** //
    m_handlers["h_help"] = &CEnhancedContext::h_help;

    // ***** GAME CONTEXT ***** //
    m_handlers["h_reloadGame"] = &CEnhancedContext::h_reloadGame;
    m_handlers["h_reloadPlayer"] = &CEnhancedContext::h_reloadPlayer;
    m_handlers["h_reloadWorlds"] = &CEnhancedContext::h_reloadWorlds;
    m_handlers["h_reloadWorld"] = &CEnhancedContext::h_reloadWorld;
    m_handlers["h_updateListeners"] = &CEnhancedContext::h_updatePlayers;

    // ***** WORLD CONTEXT ***** //
    m_handlers["h_deleteCharacter"] = &CEnhancedContext::h_deleteCharacter;
    m_handlers["h_addItem"] = &CEnhancedContext::h_addItem;
    m_handlers["h_recieveMoney"] = &CEnhancedContext::h_recieveMoney;
    m_handlers["h_eraseMoney"] = &CEnhancedContext::h_eraseMoney;
    m_handlers["h_newFight"] = &CEnhancedContext::h_newFight;
    m_handlers["h_endFight"] = &CEnhancedContext::h_endFight;
    m_handlers["h_endDialog"] = &CEnhancedContext::h_endDialog;
    m_handlers["h_gameover"] = &CEnhancedContext::h_gameover;
    m_handlers["h_addQuest"] = &CEnhancedContext::h_addQuest;
    m_handlers["h_showPersonInfo"] = &CEnhancedContext::h_showPersonInfo;
    m_handlers["h_showItemInfo"] = &CEnhancedContext::h_showItemInfo;
    m_handlers["h_changeName"] = &CEnhancedContext::h_changeName;
    m_handlers["h_setAttribute"] = &CEnhancedContext::h_setAttribute;
    m_handlers["h_setNewAttribute"] = &CEnhancedContext::h_setNewAttribute;
    m_handlers["h_addTimeEvent"] = &CEnhancedContext::h_addTimeEvent;
    m_handlers["h_setNewQuest"] = &CEnhancedContext::h_setNewQuest;
    m_handlers["h_changeDialog"] = &CEnhancedContext::h_changeDialog;

    // ***** STANDARD CONTEXT ***** //
    m_handlers["h_showExits"] = &CEnhancedContext::h_showExits;
    m_handlers["h_show"] = &CEnhancedContext::h_show;
    m_handlers["h_look"] = &CEnhancedContext::h_look;
    m_handlers["h_take"] = &CEnhancedContext::h_take;
    m_handlers["h_goTo"] = &CEnhancedContext::h_goTo;
    m_handlers["h_consume"] = &CEnhancedContext::h_consume;
    m_handlers["h_equipe"]  = &CEnhancedContext::h_equipe;
    m_handlers["h_dequipe"] = &CEnhancedContext::h_dequipe;
    m_handlers["h_examine"] = &CEnhancedContext::h_examine;
    m_handlers["h_startDialog"] = &CEnhancedContext::h_startDialog;
    m_handlers["h_changeMode"]  = &CEnhancedContext::h_changeMode;
    m_handlers["h_try"] = &CEnhancedContext::h_try;

    m_handlers["h_firstZombieAttack"] = &CEnhancedContext::h_firstZombieAttack;
    m_handlers["h_moveToHospital"] = &CEnhancedContext::h_moveToHospital;
    m_handlers["h_exitTrainstation"] = &CEnhancedContext::h_exitTrainstation;
    m_handlers["h_thieve"] = &CEnhancedContext::h_thieve;

    m_handlers["h_test"] = &CEnhancedContext::h_test;

    // *** FIGHT CONTEXT *** //
    m_handlers["h_fight"] = &CEnhancedContext::h_fight;
    m_handlers["h_fight_show"] = &CEnhancedContext::h_fight_show;

    // *** DIALOG CONTEXT *** //
    m_handlers["h_call"] = &CEnhancedContext::h_call;

    // *** TRADECONTEXT *** //
    m_handlers["h_sell"] = &CEnhancedContext::h_sell;
    m_handlers["h_buy"] = &CEnhancedContext::h_buy;
    m_handlers["h_exit"] = &CEnhancedContext::h_exit;

    // *** CHATCONTEXT *** //
    m_handlers["h_send"] = &CEnhancedContext::h_send;
    m_handlers["h_end"] = &CEnhancedContext::h_end;

    // *** QUESTS *** //
    m_handlers["0tut_hallo"] = &CEnhancedContext::h_startTutorial;
    m_handlers["1ticketverkaeufer"] = &CEnhancedContext::h_ticketverkaeufer;
    m_handlers["2ticketkauf"] = &CEnhancedContext::h_ticketverkauf;
    m_handlers["3zum_gleis"] = &CEnhancedContext::h_zum_gleis;
    m_handlers["1reden"] = &CEnhancedContext::h_reden;
    m_handlers["1besiege_besoffene_frau"] = &CEnhancedContext::h_besiege_besoffene_frau;
    m_handlers["1geldauftreiben"] = &CEnhancedContext::h_geldauftreiben;

    // *** PROGRAMMER *** //

    // *** OTHER CONTEXT *** //
    m_handlers["h_select"] = &CEnhancedContext::h_select;
    m_handlers["h_choose_equipe"] = &CEnhancedContext::h_choose_equipe;
    m_handlers["h_updateStats"] = &CEnhancedContext::h_updateStats;


}

void CEnhancedContext::initializeTemplates()
{
    m_templates["game"] = {
                    {"name", "game"}, {"permeable", true}, 
                    {"handlers",{
                        {"reload_game",{"h_reloadGame"}},
                        {"reload_player",{"h_reloadPlayer"}},
                        {"reload_world", {"h_reloadWorld"}},
                        {"reload_worlds",{"h_reloadWorlds"}},
                        {"update_players",{"h_updatePlayers"}}}}
                    };

    m_templates["world"] = {
                    {"name", "world"}, {"permeable", true},
                    {"handlers",{
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
                        {"changeDialog", {"h_changeDialog"}} }}
                    };

    m_templates["standard"] = {
                    {"name", "standard"}, {"permeable",false}, {"help","standard.txt"},   
                    {"handlers",{
                        {"go", {"h_goTo"}},
                        {"show",{"h_show"}}, 
                        {"look",{"h_look"}}, 
                        {"talk",{"h_startDialog"}}, 
                        {"pick",{"h_take"}}, 
                        {"consume",{"h_consume"}}, 
                        {"equipe",{"h_equipe"}}, 
                        {"dequipe",{"h_dequipe"}}, 
                        {"examine",{"h_examine"}}, 
                        {"mode",{"h_changeMode"}}, 
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

    m_templates["room"] = {{"name", "room"}, {"permeable", true}};
}

// ***** ***** FUNCTIONS ***** ****** // 

void CEnhancedContext::add_listener(std::string sID, std::string sEventType, size_t priority)
{
    m_eventmanager.insert(new CListener(sID, sEventType), priority, sID);
}

void CEnhancedContext::add_listener(std::string sID, std::regex eventType, int pos, size_t priority)
{
    m_eventmanager.insert(new CListener(sID, eventType, pos), priority, sID);
}

void CEnhancedContext::add_listener(std::string sID, std::vector<std::string> eventType, size_t priority)
{
    m_eventmanager.insert(new CListener(sID, eventType), priority, sID);
}

void CEnhancedContext::add_listener(std::string sID, std::map<std::string, std::string> eventType, size_t priority)
{
    m_eventmanager.insert(new CListener(sID, eventType), priority, sID);
}

void CEnhancedContext::initializeHandlers(std::vector<nlohmann::json> listeners)
{
    for(auto it : listeners)
    {
        std::cout << it << std::endl;
        if(it.count("regex") > 0)
            add_listener(it["id"], (std::regex)it["regex"], it.value("take", 1), 1); 
        else if(it.count("string") > 0)
            add_listener(it["id"], (std::string)it["string"], it.value("priority", 0));
    }
}


// *** Throw events *** //
bool CEnhancedContext::throw_event(event e, CPlayer* p)
{    
    m_curPermeable = m_permeable;
    m_block = false;
    bool called = false;
    
    std::deque<CListener*> sortedEventmanager = m_eventmanager.getSortedCtxList();
    std::reverse(sortedEventmanager.begin(), sortedEventmanager.end());
    for(size_t i=0; i<sortedEventmanager.size() && m_block == false; i++)
    {
        if(sortedEventmanager[i]->checkMatch(e) == true) {
            if(m_handlers.count(sortedEventmanager[i]->getID()) > 0)
                (this->*m_handlers[sortedEventmanager[i]->getID()])(e.second, p);
            else
                p->printError("ERROR, given handler not found: " + sortedEventmanager[i]->getID() + "\n");
            called = true;
        }
    }   
 
    if(called == false)
        error(p);
        
    return m_curPermeable;
}

// ***** ERROR FUNCTIONS ****** ***** //

void CEnhancedContext::error(CPlayer* p)
{
    if(m_permeable == false && m_sError != "")
        p->appendErrorPrint(m_sError);
    else if(m_permeable == false)
        p->appendTechPrint("Falsche Eingabe, gebe \"help\" ein, falls du nicht weiter weißt. (" +m_sName+")");
    else
        std::cout << "Context " << m_sName << " called with error state, probably normal.\n";
}

void CEnhancedContext::error_delete(CPlayer* p)
{
    std::cout << "Error from context " << m_sName << " called. Context will be deleted.\n";
    p->getContexts().erase(m_sName); 
}


// ***** ***** ***** HANDLER ***** ***** ***** //

void CEnhancedContext::h_help(std::string &sIdentifier, CPlayer* p)
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

void CEnhancedContext::h_reloadGame(std::string&, CPlayer* p)
{
    p->appendPrint("reloading game... \n");
    m_curPermeable = false;
}

void CEnhancedContext::h_reloadPlayer(std::string& sPlayer, CPlayer*p)
{
    p->appendPrint("reloading Player: " + sPlayer + "... \n");
    if(m_game->reloadPlayer(sPlayer) == false)
        p->appendPrint("Player does not exist... reloading world failed.\n");
    else
        p->appendPrint("Done.\n");
    m_curPermeable = false;
}

void CEnhancedContext::h_reloadWorlds(std::string& sPlayer, CPlayer*p)
{
    p->appendPrint("reloading all worlds... \n");
    if(m_game->reloadWorld() == true)
        p->appendPrint("Reloading all worlds failed.\n");
    else
        p->appendPrint("Done.\n");
    m_curPermeable = false;
}

void CEnhancedContext::h_reloadWorld(std::string& sPlayer, CPlayer*p)
{
    p->appendPrint("reloading world of: " + sPlayer + "... \n");
    if(m_game->reloadWorld(sPlayer) == false)
        p->appendPrint("Player does not exist... reloading world failed.\n");
    else
        p->appendPrint("Done.\n"); 
    m_curPermeable = false;
}

void CEnhancedContext::h_updatePlayers(std::string&, CPlayer*p)
{
    p->appendPrint("updating players... \n");
    m_game->playerFactory(true); 
    p->appendPrint("done.\n");
    m_curPermeable = false;
}


// ***** ***** WORLD CONTEXT ***** ***** //

void CEnhancedContext::h_deleteCharacter(std::string& sIdentifier, CPlayer* p) {
    p->getRoom()->getCharacters().erase(sIdentifier);
    delete p->getWorld()->getCharacter(sIdentifier);
    p->getWorld()->getCharacters().erase(sIdentifier); 

    m_curPermeable=false;
}

void CEnhancedContext::h_addItem(std::string& sIdentifier, CPlayer* p) {
    p->addItem(p->getWorld()->getItem(sIdentifier, p));
    m_curPermeable=false;
}

void CEnhancedContext::h_recieveMoney(std::string& sIdentifier, CPlayer* p) {
    p->setStat("gold", p->getStat("gold") + stoi(sIdentifier));
    p->appendPrint(Webcmd::set_color(Webcmd::color::GREEN) + "+" + sIdentifier + " Schiling" + Webcmd::set_color(Webcmd::color::WHITE) + "\n");

   m_curPermeable = false; 
}

void CEnhancedContext::h_eraseMoney(std::string& sIdentifier, CPlayer* p) {
    p->setStat("gold", p->getStat("gold") - stoi(sIdentifier));
    p->appendPrint(Webcmd::set_color(Webcmd::color::RED) + "-" + sIdentifier + " Schiling" + Webcmd::set_color(Webcmd::color::WHITE) + "\n");

   m_curPermeable = false; 
}
void CEnhancedContext::h_endFight(std::string& sIdentifier, CPlayer* p) {
    p->endFight();
    m_curPermeable=false;
}

void CEnhancedContext::h_endDialog(std::string& sIdentifier, CPlayer* p) {
    p->getContexts().erase("dialog");
    m_curPermeable=false;
}

void CEnhancedContext::h_newFight(std::string& sIdentifier, CPlayer* p) {
    p->setFight(new CFight(p, p->getWorld()->getCharacter(sIdentifier)));
    m_curPermeable=false;
}

void CEnhancedContext::h_gameover(std::string& sIdentifier, CPlayer* p) {
    p->setStat("hp", 0);
    p->appendStoryPrint("\nDu bist gestorben... \n\n $\n");
}

void CEnhancedContext::h_addQuest(std::string& sIdentifier, CPlayer* p) {
    p->setNewQuest(sIdentifier);
    m_curPermeable=false;
}

void CEnhancedContext::h_showPersonInfo(std::string& sIdentifier, CPlayer* p) {
    auto lambda = [](CPerson* person) { return person->getName();};
    std::string character = func::getObjectId(p->getRoom()->getCharacters(), sIdentifier, lambda);

    if(character != "")
        p->appendPrint(p->getWorld()->getCharacter(character)->getAllInformation());
    else
        p->appendPrint("character not found.\n");
    m_curPermeable=false;
}
void CEnhancedContext::h_showItemInfo(std::string& sIdentifier, CPlayer* p) 
{
    auto lambda = [](CItem* item) { return item->getName(); };
    std::string item = func::getObjectId(p->getRoom()->getItems(), sIdentifier, lambda);

    if(item != "")
        p->appendPrint(p->getRoom()->getItems()[item]->getAllInfos());
    else
        p->appendPrint("Item not found.\n");
    m_curPermeable=false;
}
void CEnhancedContext::h_changeName(std::string& sIdentifier, CPlayer* p)
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

void CEnhancedContext::h_setAttribute(std::string& sIdentifier, CPlayer* p)
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

void CEnhancedContext::h_setNewAttribute(std::string& sIdentifier, CPlayer* p)
{
    std::vector<std::string> atts = func::split(sIdentifier, "|");
    
    //Check if sIdentifier contains the fitting values
    if(atts.size() != 2 || std::isdigit(atts[1][0]) == false)
        std::cout << "Something went worng! Player Attribute could not be changed.\n";
    else
        p->getStats()[atts[0]] = stoi(atts[1]);
    m_curPermeable=false;
}

void CEnhancedContext::h_addTimeEvent(std::string& sIdentifier, CPlayer* p)
{
    std::vector<std::string> atts = func::split(sIdentifier, ",");

    //Check if sIdentifier contains the fitting values
    if(atts.size() != 2 || std::isdigit(atts[1][0]) == false)
        std::cout << "Something went worng! Player Attribute could not be changed.\n";
    else
        p->addTimeEvent("e", std::stod(atts[1], nullptr), &CPlayer::t_throwEvent, atts[0]);
    m_curPermeable = false;
}

void CEnhancedContext::h_setNewQuest(std::string& sIdentifier, CPlayer* p)
{
    p->setNewQuest(sIdentifier);
    m_curPermeable = false;
}

void CEnhancedContext::h_changeDialog(std::string& sIdentifier, CPlayer* p)
{
    std::vector<std::string> atts = func::split(sIdentifier, "|");
    if(atts.size() != 2)
    {
        std::cout << "Something went worng! Dialog could not be changed.\n";
        return;
    }

    auto lambda = [](CPerson* person) { return person->getName(); };
    std::string character = func::getObjectId(p->getRoom()->getCharacters(), atts[0], lambda);
    if(character == "")
        character = atts[0];
    p->getWorld()->getCharacter(character)->setDialog(p->getWorld()->getDialog(atts[1]));
    
    m_curPermeable = false;
}

// ***** ***** STANDARD CONTEXT ***** ***** //


void CEnhancedContext::h_showExits(std::string& sIdentifier, CPlayer* p) {
    p->appendDescPrint(p->getRoom()->showExits(p->getMode(), p->getGramma())+"\n");
        p->addSelectContest(p->getRoom()->getExtits2(), "go");
}


void CEnhancedContext::h_show(std::string& sIdentifier, CPlayer* p) {
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

void CEnhancedContext::h_look(std::string& sIdentifier, CPlayer* p) 
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
    std::string sDetail = func::getObjectId(p->getRoom()->getDetails(), sIdentifier, lambda);

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

void CEnhancedContext::h_goTo(std::string& sIdentifier, CPlayer* p) {
    p->changeRoom(sIdentifier);
}

void CEnhancedContext::h_startDialog(std::string& sIdentifier, CPlayer* p)
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

void CEnhancedContext::h_take(std::string& sIdentifier, CPlayer* p) {
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

void CEnhancedContext::h_consume(std::string& sIdentifier, CPlayer* p) {
    if(p->getInventory().getItem(sIdentifier) != NULL) {
        if(p->getInventory().getItem(sIdentifier)->callFunction(p) == false)
            p->appendTechPrint("Dieser Gegenstand kann nicht konsumiert werden.\n");
    }
    else
        p->appendErrorPrint("Gegenstand nicht in deinem Inventar! (benutze \"zeige Inventar\" um deine Items zu sehen.)\n");
}

void CEnhancedContext::h_equipe(std::string& sIdentifier, CPlayer* p) {
    if(p->getInventory().getItem(sIdentifier) != NULL) {
        if(p->getInventory().getItem(sIdentifier)->callFunction(p) == false)
            p->appendErrorPrint("Dieser Gegenstand kann nicht ausgerüstet werden.\n");
    }
    else
        p->appendErrorPrint("Gegenstand nicht in deinem Inventar! (benutze \"zeige Inventar\" um deine Items zu sehen.)\n");
}

void CEnhancedContext::h_dequipe(std::string& sIdentifier, CPlayer* p) {
    p->dequipeItem(sIdentifier);
}

void CEnhancedContext::h_examine(std::string &sIdentifier, CPlayer*p) {
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

void CEnhancedContext::h_changeMode(std::string& sIdentifier, CPlayer* p) {
    p->changeMode();
}

void CEnhancedContext::h_test(std::string& sIdentifier, CPlayer* p) {
    p->appendPrint(sIdentifier);
}


// **** SPECIAL HANDLER ***** //
void CEnhancedContext::h_firstZombieAttack(std::string& sIdentifier, CPlayer* p)
{
    //Get selected room
    if(p->getRoom()->getID() != "hospital_stairs")
        return;

    p->appendPrint("$ ");
    p->appendDescPrint("A zombie comes running down the stairs and attacks you!");

    //Create fight
    p->setFight(new CFight(p, p->getWorld()->getCharacter("hospital_hospital_stairs_zombie")));
    m_eventmanager.erase("h_firstZombieAttack");
}

void CEnhancedContext::h_moveToHospital(std::string& sIdentifier, CPlayer* p)
{
    //Get selected room
    auto lambda = [](CExit* exit) { return exit->getPrep() + "_" + exit->getName();};
    if(p->getRoom()->getID().find("compartment") == std::string::npos || func::getObjectId(p->getRoom()->getExtits(), sIdentifier, lambda) != "trainCorridor")
        return;

    p->changeRoom(p->getWorld()->getRoom("hospital_foyer"));
    m_block = true;
    m_curPermeable=false;
}

void CEnhancedContext::h_exitTrainstation(std::string& sIdentifier, CPlayer* p)
{
    std::cout << "h_exitTrainstation, " << sIdentifier << std::endl;
    std::cout << p->getRoom()->getID() << std::endl;
    std::cout << "get id: " << func::getObjectId(p->getRoom()->getExtits2(), sIdentifier) << std::endl;
    auto lambda= [](CExit* exit) { return exit->getPrep() + " " + exit->getName(); };
    if(p->getRoom()->getID() != "bahnhof_eingangshalle" || func::getObjectId(p->getRoom()->getExtits(), sIdentifier, lambda) != "ausgang")
        return;

    p->appendStoryPrint("Du drehst dich zum dem großen, offen stehenden Eingangstor der Bahnhofshalle. Und kurz kommt dir der Gedanke doch den Zug nicht zu nehmen, doch alles beim Alten zu belassen. Doch etwas sagt dir, dass es einen guten Grund gab das nicht zu tun, einen guten Grund nach Moskau zu fahren. Und auch, wenn du ihn gerade nicht mehr erkennst. Vielleicht ist gerade das der beste Grund: rausfinden, was dich dazu getrieben hat, diesen termin in Moskau zu vereinbaren.\n Du guckst dich wieder in der Halle um, und überlegst, wo du anfängst zu suchen.\n");
    
    m_block=true;
}

void CEnhancedContext::h_thieve(std::string& sIdentifier, CPlayer* p)
{
    p->appendStoryPrint("You know, me son. You should not be stealing!");
    m_curPermeable = false;
}



void CEnhancedContext::h_try(std::string& sIdentifier, CPlayer* p)
{
    p->throw_events("recieveMoney 4", "try");
    p->throw_events("go neben", "try");
    p->throw_events("go Toil", "try");
    p->throw_events("gehe zur Männer", "try");
    p->throw_events("spreche Ticket an", "try");

}


// ***** ***** FIGHT CONTEXT ***** ***** //
void CEnhancedContext::initializeFightListeners(std::map<std::string, std::string> mapAttacks)
{
    add_listener("h_fight", mapAttacks, 1);
}

void CEnhancedContext::h_fight(std::string& sIdentifier, CPlayer* p) {
    std::string newCommand = p->getFight()->fightRound((sIdentifier));
    if(newCommand != "")    
        p->throw_events(newCommand, "h_fight");
}

void CEnhancedContext::h_fight_show(std::string& sIdentifier, CPlayer* p) {
    if(sIdentifier == "stats")
        p->appendPrint(p->getFight()->printStats(p));
    else if(sIdentifier == "opponent stats")
        p->appendPrint(p->getFight()->printStats(p->getFight()->getOpponent()));
    else
        p->appendPrint("Falsche Eingabe! Versuche es mit \"show stats\" oder \"show opponent stats\"\n");
}


// ***** ***** DIALOG CONTEXT ***** *****

void CEnhancedContext::initializeDialogListeners(std::string newState, CPlayer* p)
{
    //Set (new) state.
    setAttribute<std::string>("state", newState); 

    //Clear listeners.
    m_eventmanager.clear();

    //Add help listener 
    add_listener("h_help", "help", 1);

    //Add listener for each dialog option.
    std::vector<size_t> activeOptions = p->getDialog()->getState(newState)->getActiveOptions(p);
    std::map<size_t, size_t> mapOtptions;
    size_t counter = 1;
    for(auto opt : activeOptions)
    {
        mapOtptions[counter] = opt;
        add_listener("h_call", std::to_string(counter), 1);
        counter++;
    }
    setAttribute<std::map<size_t, size_t>>("mapOptions", mapOtptions);
    std::cout << "Done.\n";
}

void CEnhancedContext::h_call(std::string& sIdentifier, CPlayer* p)
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

void CEnhancedContext::print(CPlayer* p)
{
    CPerson* partner = p->getWorld()->getCharacter(getAttribute<std::string>("partner"));
    p->appendPrint("<b>" + p->getName() + "s Inventar:</b>\n" 
                    + p->getInventory().printInventory()
                    + "\n<b>" + partner->getName() + "s Inventar: </b>\n" 
                    + partner->getInventory().printInventory());
}

void CEnhancedContext::h_sell(std::string& sIdentifier, CPlayer* p)
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

void CEnhancedContext::h_buy(std::string& sIdentifier, CPlayer* p)
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

void CEnhancedContext::h_exit(std::string&, CPlayer* p)
{
    CPerson* partner = p->getWorld()->getCharacter(getAttribute<std::string>("partner"));
    p->getContexts().erase("trade");
    p->getContexts().erase("dialog");
    p->startDialog(partner->getID());
}

// ***** ***** CHAT CONTEXT ***** ***** //
void CEnhancedContext::h_send(string& sInput, CPlayer* p)
{
    CPlayer* chatPartner = p->getPlayer(getAttribute<std::string>("partner"));
    chatPartner->send(p->returnSpeakerPrint(func::returnToUpper(p->getName()), sInput + "\n"));
    p->appendSpeackerPrint("YOU", sInput + "\n");
}

void CEnhancedContext::h_end(string& sMessage, CPlayer* p)
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


// ***** ***** QUEST CONTEXT ***** ***** //

void CEnhancedContext::initializeQuestListeners(map_type handler)
{
    for(auto it : handler)
    {
        if(m_handlers.count(it.first) == 0)
            std::cout << "FATAL ERROR, Quest-handler \"" << it.first << "\" does not exits!!\n";
        else
            add_listener(it.first, it.second, 1);
    }
}


// *** *** Tutorial *** *** //
void CEnhancedContext::h_startTutorial(std::string&, CPlayer* p)
{
    p->appendStoryPrint("Willkommen bei \"DER ZUG\"! Du befindest dich auf dem Weg nach Moskau. Dir fehlt dein Ticket. Tickets sind teuer. Glücklicherweise kennst du einen leicht verrückten, viel denkenden Mann, der sich \"Der Ticketverkäufer\" nennt. Suche ihn, er hat immer noch ein günsttiges Ticket für dich. Benutze die Befhelte \"go to [name des Ausgangs]\", um den Raum zu wechseln, um dir Personen und Ausgänge anzeigen zu lassen, nutze \"show people\", bzw. \"show exits\" oder auch \"show all\". Eine Liste mit allen Befhelen und zusätzlichen Hilfestellungen erhältst du, indem du \"help\" eingibst.$ ");

    p->setNewQuest("zug_nach_moskau");
    m_curPermeable = false;
}

// *** *** Zug nach Moskau *** *** //
void CEnhancedContext::h_ticketverkaeufer(std::string& sIdentifier, CPlayer* p)
{
    std::cout << "Called, with id: " << sIdentifier << std::endl;
    if(p->getRoom()->getID() != "bahnhof_toiletten")
        return;

    std::cout << "Called, with id: " << sIdentifier << std::endl;

    if(sIdentifier == "bahnhof_maennerToilette" || fuzzy::fuzzy_cmp("zur männer-toilette", sIdentifier) <= 0.2) {
        p->questSolved(getAttribute<std::string>("questID"), "1ticketverkaeufer");
        m_eventmanager.erase("1ticketverkaeufer");
    }
}


void CEnhancedContext::h_ticketverkauf(std::string& sIdentifier, CPlayer* p)
{
    std::cout << "h_ticketverkauf, " << sIdentifier << std::endl;
    if(sIdentifier == "ticket")
        p->questSolved(getAttribute<std::string>("questID"), "2ticketkauf");
}

void CEnhancedContext::h_zum_gleis(std::string& sIdentifier, CPlayer* p)
{    
    auto lamda= [](CExit* exit) {return exit->getPrep() + " " + exit->getName();};
    std::cout<< "sIdentifier: " << sIdentifier << std::endl;
    std::string room = func::getObjectId(p->getRoom()->getExtits(), sIdentifier, lamda);

    if(room != "gleis3" || p->getInventory().getItem_byID("ticket") == NULL)
        return; 

    p->questSolved(getAttribute<std::string>("questID"), "3zum_gleis");

    p->appendStoryPrint("Du siehst deinen Zug einfahren. Du bewegst dich auf ihn zu, zeigst dein Ticket, der Schaffner musstert dich kurz und lässt dich dann eintreten. Du suchst dir einen freien Platz, legst dein Bündel auf den sitz neben dich und schläfst ein...\n $ Nach einem scheinbar endlos langem schlaf wachst du wieder in deinem Abteil auf. Das Abteil ist leer. Leer bist auf einen geheimnisvollen Begleiter: Parsen.\n");

    p->setRoom(p->getWorld()->getRoom("compartment-a"));
    m_curPermeable = false;
    m_block = true;
    p->getContexts().erase(getAttribute<std::string>("questID"));
    p->questSolved("tutorial", "1tutorial");
}

// *** *** Die komische Gruppe *** *** //
void CEnhancedContext::h_reden(std::string& sIdentifier, CPlayer* p)
{
    auto lambda = [](CPerson* person) { return person->getName();};
    std::string character = func::getObjectId(p->getRoom()->getCharacters(), sIdentifier, lambda);
    if(character == "" || character.find("passant") == std::string::npos)
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
        for(auto it : {"", "2", "3"})
            p->getWorld()->getCharacter((string)"trainstation_bahnhof_eingangshalle_passant"+it)->setDialog(p->getWorld()->getDialog("strangeGuysDialog2"));
        for(auto it : {"", "2", "3"})
            p->getWorld()->getCharacter((string)"trainstation_bahnhof_nebenhalle_passant"+it)->setDialog(p->getWorld()->getDialog("strangeGuysDialog2"));
        for(auto it : {"", "2", "3"})
            p->getWorld()->getCharacter((string)"trainstation_gleis5_passant"+it)->setDialog(p->getWorld()->getDialog("strangeGuysDialog2"));

        p->getContexts().erase(quest->getID());
    }
}

// *** *** Besoffene Frau *** *** //
void CEnhancedContext::h_besiege_besoffene_frau(std::string& sIdentifier, CPlayer* p)
{
    if(sIdentifier != "trainstation_bahnhof_frauenToilette_besoffene_frau")
        return;

    p->questSolved(getAttribute<std::string>("questID"), "1besiege_besoffene_frau");
    p->appendDescPrint("Du suchst in den Taschen der Frau und findest drei Schillinge.\n");
    p->throw_events("recieveMoney 3", "h_besiege_besoffene_frau");
    p->getContexts().erase(getAttribute<std::string>("questID"));
}

// *** *** GELD AUFTREIBEN *** *** //
void CEnhancedContext::h_geldauftreiben(std::string& sIdentifier, CPlayer* p)
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

void CEnhancedContext::h_select(std::string& sIdentifier, CPlayer* p)
{
    map_type map_objects = getAttribute<map_type>("map_objects");
    std::string obj = func::getObjectId(map_objects, sIdentifier);

    p->throw_events(getAttribute<std::string>("eventtype")+ " " + obj, "h_select");
    m_curPermeable=false;
    p->getContexts().erase("select"); 
}

void CEnhancedContext::h_choose_equipe(std::string& sIdentifier, CPlayer* p)
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

void CEnhancedContext::h_updateStats(std::string& sIdentifier, CPlayer* p)
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

