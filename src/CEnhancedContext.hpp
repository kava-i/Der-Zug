#ifndef CENHANCEDCONTEXT_H 
#define CENHANCEDCONTEXT_H

#include <iostream>
#include <map>
#include <vector>
#include "func.hpp"
#include "Webcmd.hpp"
#include "SortedContext.hpp"
#include "CListener.hpp"
#include "json.hpp"

class CPlayer;
class CGame;

class CEnhancedContext
{
protected:
    typedef std::map<std::string, std::string> map_type; typedef std::pair<std::string, std::string> event;

    CContextStack<CListener> m_eventmanager;
    
    nlohmann::json m_jAttributes;
    std::string m_sName;
    bool m_permeable;
    bool m_curPermeable;
    bool m_block;
    std::string m_sHelp;
    std::string m_sError;

    CGame* m_game;

    void(CEnhancedContext::*m_error)(CPlayer*);

    static std::map<std::string, void(CEnhancedContext::*)(std::string&,CPlayer*)> m_handlers;
    static std::map<std::string, nlohmann::json> m_templates;

public:

    CEnhancedContext(nlohmann::json jAttributes);
    CEnhancedContext(std::string sTemplate);
    CEnhancedContext(std::string sTemplate, nlohmann::json jAttributes);

    // *** GETTER *** //
    std::string getName();
    bool getPermeable();
    nlohmann::json getTemplate(std::string sTemplate);

    template<typename T> 
    T getAttribute(std::string sAttribute)
    {
        if(m_jAttributes.count(sAttribute) == 0)
            std::cout << "Attributes requested that does not exist: " << sAttribute << std::endl;
        return m_jAttributes.value(sAttribute, T());
    }

    // *** SETTER *** //
    void setName(std::string sName);
    void setGame(CGame* game);
    void setErrorFunction(void(CEnhancedContext::*func)(CPlayer*));

    template<typename T> 
    T setAttribute(std::string sAttribute, T attribute)
    {
        if(m_jAttributes.count(sAttribute) == 0)
            std::cout << "About to set attribute that does not exist: " << sAttribute << std::endl;
        return m_jAttributes[sAttribute] = attribute;
    }

    // *** initializer *** //
    static void initializeHanlders();
    static void initializeTemplates();
    

    // *** Throw events *** //
    void add_listener(std::string sID, std::string sEventType, size_t priority=0);
    void add_listener(std::string sID, std::regex eventType, int pos, size_t priority=0);
    void add_listener(std::string sID, std::vector<std::string> eventType, size_t priority=0);
    void add_listener(std::string sID, std::map<std::string, std::string> eventType, size_t priority=0);

    void initializeHandlers(std::vector<nlohmann::json> handlers);

    bool throw_event(event newEvent, CPlayer* p);

    // *** ERROR HANDLER *** //
    void error(CPlayer* p);
    void error_delete(CPlayer* p);

    // *** BASIC HANDLER *** //
    void h_help(std::string&, CPlayer*);

    // *** GAME CONTEXT *** //
    void h_reloadGame(std::string&, CPlayer*);
    void h_reloadPlayer(std::string&, CPlayer*);
    void h_reloadWorlds(std::string&, CPlayer*);
    void h_reloadWorld(std::string&, CPlayer*);
    void h_updatePlayers(std::string&, CPlayer*);

    // *** WORLD CONTEXT *** //
    void h_deleteCharacter(std::string&, CPlayer*);
    void h_addItem(std::string&, CPlayer*);
    void h_recieveMoney(std::string&, CPlayer*);
    void h_eraseMoney(std::string&, CPlayer*);
    void h_newFight(std::string&, CPlayer*);
    void h_endFight(std::string&, CPlayer*);
    void h_endDialog(std::string&, CPlayer*);
    void h_gameover(std::string&, CPlayer*);
    void h_addQuest(std::string&, CPlayer*);
    void h_showPersonInfo(std::string&, CPlayer*);
    void h_showItemInfo(std::string&, CPlayer*);
    void h_changeName(std::string&, CPlayer*);
    void h_setAttribute(std::string&, CPlayer*);
    void h_setNewAttribute(std::string&, CPlayer*);
    void h_addTimeEvent(std::string&, CPlayer*);
    void h_setNewQuest(std::string&, CPlayer*);

    // *** STANDARD CONTEXT *** //
    void h_showExits(std::string& sIdentifier, CPlayer* p);
    void h_show             (std::string&, CPlayer*);
    void h_look             (std::string&, CPlayer*);
    void h_take             (std::string&, CPlayer*);
    void h_consume          (std::string&, CPlayer*);
    void h_equipe           (std::string&, CPlayer*);
    void h_dequipe          (std::string&, CPlayer*);
    void h_examine          (std::string&, CPlayer*);
    void h_goTo             (std::string&, CPlayer*);
    void h_startDialog      (std::string&, CPlayer*);
    void h_changeMode       (std::string&, CPlayer*);
    void h_try(std::string&, CPlayer*);                 ///< PRogrammer function for testing
    void h_test(std::string&, CPlayer*);

    //Rooms
    void h_firstZombieAttack (std::string&, CPlayer*);
    void h_moveToHospital    (std::string&, CPlayer*);
    void h_exitTrainstation  (std::string&, CPlayer*);

    //Tuturial
    void h_startTutorial     (std::string&, CPlayer*);


    // *** FIGHT CONTEXT *** //
    void initializeFightListeners(std::map<std::string, std::string> mapAttacks);
    void h_fight(std::string&, CPlayer*);
    void h_fight_show(std::string&, CPlayer*);

    // *** DIALOG CONTEXT *** //
    void initializeDialogListeners(std::string newState, CPlayer* p);
    void h_call(std::string&, CPlayer*);

    // *** TRADECONTEXT *** //
    void print(CPlayer* p);
    void h_sell(std::string&, CPlayer*);
    void h_buy(std::string&, CPlayer*);
    void h_exit(std::string&, CPlayer*);

    // *** CHATCONTEXT *** //
    void h_send(std::string&, CPlayer*);
    void h_end(std::string&, CPlayer*);

    // *** QUESTS *** //
    void initializeQuestListeners(std::map<std::string, std::string> handler);
    void h_ticketverkaeufer(std::string&, CPlayer*);
    void h_ticketverkauf(std::string&, CPlayer*);
    void h_zum_gleis(std::string&, CPlayer*);
    void h_reden(std::string&, CPlayer*);
    void h_besiege_besoffene_frau(std::string&, CPlayer*);
    void h_geldauftreiben(std::string&, CPlayer*);

    // *** OTHER CONTEXTS *** //
    void h_select(std::string&, CPlayer*);
    void h_choose_equipe(std::string&, CPlayer*);
    void h_updateStats(std::string&, CPlayer*);
};
    
#endif 
