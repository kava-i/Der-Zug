#include "CEnhancedContext.hpp"

// ***** ***** CONSTRUCTORS ***** ***** //

CEnhancedContext::CEnhancedContext(nlohmann::json jAttributes)
{
    m_jAttributes = jAttributes;
    m_sName = jAttributes.value("name", "context");
    m_permeable = jAttributes("permeable", false);
    m_curPermeable = m_permeable;
    m_block = false;
    m_sHelp = jAttributes.value("help", "");
    m_sError = jAttributes.value("error", "");

    if(jAttributes.count("handlers") > 0)
    {
        for(const auto &it : jAttributes["handlers"])
            add_listener(it.first, m_handlers[it.second]);  
    }
    add_listener("help", &h_help);
}
 
CEnhancedContext::CEnhancedContext(std::string sTemplate)
{
    if(m_templates.count(sTemplate) > 0)
        CEnhancedContext(m_templates[sTemplate]);
    else
        std::cout << "Error in creating context with template: " << sTemplate << std::endl;
}

CEnhancedContext::CEnhancedContext(std::string sTemplate, nlohmann::json jAttributes)
{
    if(m_templates.count(sTemplate) > 0)
        CEnhancedContext(m_templates[sTemplate]);
    else
        std::cout << "Error in creating context with template: " << sTemplate << std::endl;

    m_jAttributes = jAttributes;
    m_sName = jAttributes.value("name", m_sName);
    m_permeable = jAttributes("permeable", m_permeable);
    m_curPermeable = m_permeable;
    m_sHelp = jAttributes.value("help", m_sHelp);
    m_sError = jAttributes.value("error", m_sError);

    if(jAttributes.count("handlers") > 0)
    {
        for(const auto &it : jAttributes["handlers"])
            add_listener(it.first, m_handlers[it.second]);  
    }
}

// ***** ***** INITIALIZERS ***** ***** //
std::map<string, void (CContext::*)(std::string&, CPlayer* p)> CQuestContext::m_handlers = {};
void CQuestContext::initializeHanlders()
{

    // ***** GAME CONTEXT ***** //
    m_handlers["h_reloadGame"] = &CEnhancedContext::h_reloadGame;
    m_handlers["h_reloadPlayer"] = &CEnhancedContext::h_reloadPlayer;
    m_handlers["h_reloadWorlds"] = &CEnhancedContext::h_reloadWorlds;
    m_handlers["h_reloadWorld"] = &CEnhancedContext::h_reloadWorld;
    m_handlers["h_updateListeners"] &CEnhancedContext::h_updatePlayers;

    // ***** WORLD CONTEXT ***** //
    m_handlers["h_deleteCharacter"] = &h_deleteCharacter(string&, CPlayer*)M
    m_handlers["h_addItem"] = &h_addItem(string&, CPlayer*);
    m_handlers["h_recieveMoney"] = &h_recieveMoney(string&, CPlayer*);
    m_handlers["h_newFight"] = &h_newFight(string&, CPlayer*);
    m_handlers["h_endFight"] = &h_endFight(string&, CPlayer*);
    m_handlers["h_endDialog"] = &h_endDialog(string&, CPlayer*);
    m_handlers["h_gameover"] = &h_gameover(string&, CPlayer*);

    // *** FIGHT CONTEXT *** //
    m_handlers["h_fight_show"] = &h_fight_show(string&, CPlayer*);

    // *** DIALOG CONTEXT *** //
    m_handlers["h_call"] = &h_call(string&, CPlayer*);

    // *** TRADECONTEXT *** //
    m_handlers["h_sell"] = &h_sell(string&, CPlayer*);
    m_handlers["h_buy"] = &h_buy(string&, CPlayer*);
    m_handlers["h_exit"] = &h_exit(string&, CPlayer*);

    // *** CHATCONTEXT *** //
    m_handlers["h_send"] = &h_send(string&, CPlayer*);
    m_handlers["h_end"] = &h_end(string&, CPlayer*);

    // *** CHOICE CONTEXT *** //
    m_handlers["h_select"] = &h_select(string&, CPlayer*);
    m_handlers["h_choose_equipe"] = &h_choose_equipe(string&, CPlayer*);
    m_handlers["h_updateStats"] = &h_updateStats(string&, CPlayer*);

    // *** QUESTS *** //
    m_handlers["h_ticketverkaeufer"] = &h_ticketverkaeufer(string&, CPlayer*);
    m_handlers["h_ticketverkauf"] = &h_ticketverkauf(string&, CPlayer*);
    m_handlers["h_zum_gleis"] = &h_zum_gleis(string&, CPlayer*);
    m_handlers["h_reden"] = &h_reden(string&, CPlayer*);
    m_handlers["h_besiege_besoffene_frau"] = &h_besiege_besoffene_frau(string&, CPlayer*);
    m_handlers["h_geldauftreiben"] = &h_geldauftreiben(string&, CPlayer*)

    // *** PROGRAMMER *** //
    m_handlers["h_try"] = &h_try(string&, CPlayer*) {}

    // ***** QUESTS ***** //

    //Use id of state as identfier

    // *** Zug nach Moskau *** //
    m_handlers["1ticketverkaeufer"] = &CContext::h_ticketverkaeufer;
    m_handlers["2ticketkauf"]       = &CContext::h_ticketverkauf;
    m_handlers["3zum_gleis"]        = &CContext::h_zum_gleis;

    // *** Die komische Gruppe *** // 
    m_handlers["1reden"] = &CContext::h_reden;
    // *** Besoffene Frau *** //
    m_handlers["1besiege_besoffene_frau"] = &CContext::h_besiege_besoffene_frau;

    // *** Geld auftreiben *** //
    m_handlers["1geldauftreiben"] = &CContext::h_geldauftreiben;
}


std::map<std::string, nlohmann::json> m_templates;
void CEnhancedContext::initializeTemplates()
{
    m_templates["standard"] = {"name":"standard", "permeable":false, "help":"standard.txt", "handlers":["show":"h_show", "look":"h_look", "go":"h_goTo", "talk":"h_startDialog", "pick":"h_take", "consume":"h_consume", "equipe":"h_equipe", "dequipe":"h_dequipe", "examine":"h_examine", "mode":"h_mode", "find":"h_find"]};
    m_templates["fight"] = {"name":"fight", "permeable":false,"help":"fight.txt", "handlers":["show":"h_fight_show"]};
    m_templates["dialog"] = {"name":"dialog","permeable":false,"help":"dialog.txt"};
    m_templates["trade"] = {"name":"trade", "permeable":false, "help":"trade.txt"};
    m_templates
