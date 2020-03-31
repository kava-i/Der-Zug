#ifndef CENHANCEDCONTEXT_H
#define CENHANCEDCONTEXT_H

#include <iostream>
#include <map>
#include <vector>
#include "func.hpp"
#include "Webcmd.hpp"
#include "json.hpp"

class CPlayer;
class CGame;

class CEnhancedContext
{
protected:
    typedef std::pair<std::string, std::string> event;
    typedef std::pair<std::string, std::vector<void(CEnhancedContext::*)(std::string, CPlayer*)>> eventmanager;

    eventmanager m_eventmanager;

    nlohmann::json m_jAttributes;
    std::string m_sName;
    bool m_permeable;
    bool m_curPermeable;
    bool m_block;
    std::string m_sHelp;
    std::string m_sError;

    /*
    std::string m_curState;
    std::string m_dialogPartner;
    std::string m_sObject;
    std::map<std::string, std::string> m_sObject;
    
    CQuest* quest;
    CPlayer* m_chatPartner;
    CCharacter* m_tradingPartner;
    
    CItem* m_curItem;
    */
    
    static std::map<std::string, void(CContext::*)(std::string&,CPlayer*)> m_handler;
    static std::map<std::string, nlohmann::json> m_templates;

public:

    CEnhancedContext(nlohmann::json jAttributes);
    CEnhancedContext(std::string sTemplate);
    CEnhancedContext(std::strinh sTemplate, nlohmann::json jAttributes);

    // *** GETTER *** //
    std::string getName();
    bool getPermeable();

    // *** SETTER *** //
    void setName(std::string sName);

    // *** initializer *** //
    static void initializeHanlders();
    static void initializeTemplates();
    

    // *** Add and delete listeners and throw event
    void add_listener(std::string sEventType, void(CContext::*)(std::string&, CPlayer*));
    void add_listener(std::string sEventType, void(CContext::*)(std::string&, CPlayer*), size_t pos);
    void delete_listener(std::string sEventType, int num);

    bool throw_event(event newEvent, CPlayer* p);

    // *** GAME CONTEXT *** //
    void h_reloadGame(string&, CPlayer*);
    void h_reloadPlayer(string&, CPlayer*);
    void h_reloadWorlds(string&, CPlayer*);
    void h_reloadWorld(string&, CPlayer*);
    void h_updatePlayers(string&, CPlayer*);

    // *** WORLD CONTEXT *** //
    void h_deleteCharacter(string&, CPlayer*)M
    void h_addItem(string&, CPlayer*);
    void h_recieveMoney(string&, CPlayer*);
    void h_newFight(string&, CPlayer*);
    void h_endFight(string&, CPlayer*);
    void h_endDialog(string&, CPlayer*);
    void h_gameover(string&, CPlayer*);

    // *** FIGHT CONTEXT *** //
    void h_fight(string&, CPlayer*);

    // *** DIALOG CONTEXT *** //
    void h_call(string&, CPlayer*);

    // *** TRADECONTEXT *** //
    void h_sell(string&, CPlayer*);
    void h_buy(string&, CPlayer*);
    void h_exit(string&, CPlayer*);

    // *** CHATCONTEXT *** //
    void h_send(string&, CPlayer*);
    void h_end(string&, CPlayer*);

    // *** CHOICE CONTEXT *** //
    void h_select(string&, CPlayer*);
    void h_choose_equipe(string&, CPlayer*);
    void h_updateStats(string&, CPlayer*);

    // *** QUESTS *** //
    void h_ticketverkaeufer(string&, CPlayer*);
    void h_ticketverkauf(string&, CPlayer*);
    void h_zum_gleis(string&, CPlayer*);
    void h_reden(string&, CPlayer*);
    void h_besiege_besoffene_frau(string&, CPlayer*);
    void h_geldauftreiben(string&, CPlayer*)

    // *** PROGRAMMER *** //
    void h_try(string&, CPlayer*) {}

};
    
    
