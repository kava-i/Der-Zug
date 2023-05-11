#ifndef CPLAYER_H 
#define CPLAYER_H

#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <stdlib.h>
#include <chrono> 
#include <time.h>
#include <ctime>

#include "eventmanager/parser.h"
#include "game/config/attributes.h"
#include "game/world.h"
#include "person.h"
#include "eventmanager/sorted_context.h"
#include "eventmanager/context.h"
#include "room.h"
#include "actions/fight.h"
#include "mind.h"
#include "tools/gramma.h"
#include "tools/func.h"


#include <JanGeschenk/Webconsole.hpp>
#include <JanGeschenk/Webgame.hpp>
 
using std::string;
using std::map;
using std::vector;

class CPlayer : public CPerson
{
private:
    
    // *** General attributes *** //
    // m_sName, m_sID (from CPerson)
    std::string m_sPassword;  ///< Password for login
    bool m_firstLogin;  ///< Indicate whether this is the players first login.
    string m_sPrint;  ///< Text printing after throwing events
    std::map<std::string, std::string> subsitutes_;

    //*** Stagged Events *** //
    std::string m_staged_pre_events;  ///< Events thrown before printing
    std::string m_staged_post_events;  ///< Events thrown after printing

    // *** Attributes concerning game world *** //
    CWorld* m_world;  ///< Pointer to players world (all rooms, chars, etc.)
    CParser* m_parser;
    static std::shared_ptr<CGramma> _gramma;  ///< Pointer to the gramma-class

    CRoom* m_room;  ///< Pointer to current room
    CRoom* m_lastRoom;  ///< Pointer to last room visited

    // *** Levels & Ep, Items, Equipment etc. *** //
    std::map<std::string, SMind> m_minds; ///< Characters in unconsciousness 
                                          ///< (auto skilled)
    std::map<std::string, CItem*> m_equipment;  ///< Equipped weapons, clothing etc.
    //m_inventory, m_attacks (from CPerson)


    // *** States, like current fight *** //
    CFight* m_curFight;  ///< Current fight player is engaged in
    CPerson* m_curDialogPartner;  ///< Current dialog partner
    std::string cur_music_;
    std::string cur_image_;

    // *** Others *** //
    map<string, CPlayer*> m_players;  ///< List of all online players in game
    std::map<std::string, bool> m_vistited;  ///< List of all visited rooms
    CContextStack<Context> m_contextStack;  ///< List of all current contexts
    Webconsole* _cout;  ///< Pointer to the console

    typedef map<string, vector<std::tuple<std::chrono::system_clock::time_point, double, void(CPlayer::*)(std::string), std::string>> > timeEvents;
    timeEvents m_timeEventes;  ///< List of all events triggered by time

public:

    // ** Constructors ** //

    /**
    * Full constructor for player
    * @param jAtts json with all attributes
    * @param room current room of payer
    * @param newAttacks attacks of player
    */
    CPlayer(nlohmann::json jAttributes, CRoom* room, attacks newAttacks, std::string path);

    ~CPlayer();

    // *** GETTER *** // 
    ///Return first login (yes, no)
    bool getFirstLogin();

    ///Return output for player (Append newline)
    string getPrint();

    ///Return pointer to players world (all rooms, chars, etc.)
    CWorld* getWorld();

    ///Return current room.
    CRoom* getRoom();

    ///Return dictionary of player's minds.
    std::map<std::string, SMind>& getMinds();

    ///Return a requested mind
    SMind& getMind(std::string sMind);

    ///Return players equipment (weapons, clothing etc.)
    std::map<std::string, CItem*>& getEquipment();

    ///Return current fight.
    CFight* getFight();

    ///Return current dialog-partner
    CPerson* getCurDialogPartner();

    ///Return players context-stack 
    CContextStack<Context>& getContexts();

    ///Return a context from players context stack
    Context* getContext(std::string);

    //Return map of players
    std::map<std::string, CPlayer*>& getMapOFOnlinePlayers();

    //Get current status
    std::map<std::string, std::string> GetCurrentStatus();

    // *** SETTER *** //
    
    ///Set first login.
    void setFirstLogin(bool val); 

    ///Set new output for player.
    void setPrint(string newPrint);

    ///Set extra substitues
    void set_subsitues(std::map<std::string, std::string> subsitutes);

    static void set_gramma(std::shared_ptr<CGramma> gramma);

    //No-event type commands
    void printText(std::string text);

    ///Append to current player output.
    void appendPrint(string newPrint, bool dont_throw_post=false);
    void appendStoryPrint(std::string sPrint);
    void appendDescPrint(std::string sPrint);
    void appendErrorPrint(std::string sPrint);
    void appendTechPrint(std::string sPrint);
    void appendSpeackerPrint(std::string sSpeaker, std::string sPrint);
    void appendBlackPrint(std::string sPrint);
    std::string returnSpeakerPrint(std::string sSpeaker, std::string sPrint);
    std::string returnBlackPrint(std::string sPrint);
    void appendSuccPrint(std::string sPrint);

    ///Add staged events to throw before printing
    void addPreEvent(std::string sNewEvent);

    ///Add staged events to throw after printing
    void addPostEvent(std::string sNewEvent);

    //Throw staged events and clear events afterwards
    void throw_staged_events(std::string& events, std::string sMessage);

    ///Set player's world.
    void setWorld(CWorld* newWorld);

    ///Set last room to current room and current room to new room.
    void setRoom(CRoom* room);

    ///Set map of all online players.
    void setPlayers(map<string, CPlayer*> players);

    ///Set webconsole.
    void setWebconsole(Webconsole* webconsole);

    // *** FUNCTIONS *** // 

    /**
    * Update room context after changing location
    */
    void updateRoomContext();
    
    void RemoveListenerFromLocation(std::string location, std::string location_id, std::string id);


    // ** Fight ** //

    /**
    * Set current fight of player and add a fight-context to context-stack
    * @param newFight new fight
    */
    void setFight(CFight* fight);

    /**
    * Delete the current fight and erase fight-context from context-stack
    */
    void endFight();


    // ** Dialog + Chat + Read ** //

    /**
    * Set current (new) Dialog player and add a Dialog-context to context-stack.
    * @param sCharacter id of dialogpartner and throw event to start Dialog.
    */
    void startDialog(string sCharacter, CDialog* dialog = nullptr);
    
    

    /**
    * Try to start chatting. If player is busy, print error message, else add chat-context to 
    * context-stack and throw event 'Hey + player-name'.
    */
    void startChat(CPlayer* sPlayer);

    /**
    * Add chat context.
    * @param sPlayer (chat partner (other player))
    */
    void addChatContext(std::string sPartner);

    /**
    * Direct print of message to web-console. Used when chatting and usually call in chat-context
    * of the Dialog partner, printing, what he said.
    * @param sMessage message to print to console
    */
    void send(string sMessage);

    /**
    * Add read context.
    * @param sItem (id of book which to read)
    */
    void addReadContext(std::string sID);

    // ** Room ** //

    /**
    * Check player input, whether 1. player wants to go back, 2. player wants to use an exit in current
    * room, or 3. player wants to go to a room already visited.
    * @param sIdentifier player input (room id, exit of current room, or room already visited)
    */
    void changeRoom(string sIdentifier);

    /**
    * Change room to given room and print entry description. Set last room to current room.
    * @param newRoom new room the player changes to
    */
    void changeRoom(CRoom* newRoom);


    /**
    * Print all already visited rooms.
    */
    void showVisited();

    /**
    * Look for shortes way (if exists) to given room. And return way as vector, or empty array.
    * @param room players room
    * @param roomID id of desired target-room
    * @return vector with way to target.
    */
    std::vector<std::string> findWay(CRoom* room, std::string sRoomdID);

    // ** Item and Inventory** //

    /**
    * Adding all (non-hidden) items in room to players inventory.
    */
    void addAll();

    /**
    * Add given item to player's inventory and print message.
    * @param item given item/ item to add to inventory.
    */
    void addItem(CItem* item);

    /**
    * Start a trade. Add a trade-context to player's context stack.
    * @param partner Player's trading partner add to context-information.
    */
    void startTrade(std::string partner);

    /**
    * Add all equipped items to player's output.
    */
    void printEquiped();

    /**
    * Equipe given item into given category (sType). If there is already an item equipped in this 
    * category, add choice-context to context-stack.
    * @param item given item.
    * @param sType type of item indicating category.
    */
    void equipeItem(CItem*, string sType);

    /**
    * Dequipe an item. Erase attack from list of attacks if it depended on equipped item.
    */
    void dequipeItem(string sType);


    // ** Quests ** //

    /**
    * show all active or solved quest depending on 'solved'
    * @param solved indicating, whether to show solved or active quests.
    */
    void showQuests(bool solved);

    /**
    * Add new quest by setting quest active.
    * @param sQuestID id to given quest.
    */
    void setNewQuest(std::string sQuestID);

    /**
    * Set a quest-step as solved and add received ep (experience points) to players ep.
    * @param sQuestID identifier to quest.
    * @param sStepID identifier to quest-step.
    */
    void questSolved(std::string sQuestID, std::string sStepID);

    // Minds and level

    /**
    * Let player know how many learning points player can assign and add choice context.
    * @param numPoints experience points player can assign.
    */
    void UpdateStats(int numPoints); 

		void PrintSkillableAttributes(int available_points);

    /**
    * Print minds of player by using table function.
    */
    void showMinds();

    /**
    * Print player's stats by using table function.
    */
    void showStats();

    /**
    * Check given dependencies. Receive a json and check whether this matches player's 
    * minds or stats.
    * @param jDeps json with dependencies
    */
    bool checkDependencies(nlohmann::json);


    // ** Others ** //

    /**
    * Check if player's output contains special commands such as printing player name or else.
    */
    void checkCommands();

    /**
    * Check whether given password and name matches this player.
    * @param sName name to compare to player's name.
    * @param sPassword password to compare to player's password.
    */
    std::string doLogin(string sName, string sPassword);

    /**
    * Get a player from currently online player's by their name using fuzzy comparison. 
    * @param sIdentifier identifier (player's name)
    */
    CPlayer* getPlayer(string sIdentifier);

    /**
    * In the case of a serious error, leading game not not continue, let the player no
    * that something went wrong and print error in console.
    */
    void printError(std::string sError);

		int CalculateWoozyness(std::vector<std::string> attributes, WoozyMethods method);


    // ** Eventmanager functions ** // 

    void updateMedia();
    std::string getContextMusic(std::string music);
    std::string getContextImage(std::string music);

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
    void throw_events(string sInput, std::string sMessage);
};

#endif
    
