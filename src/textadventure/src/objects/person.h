#ifndef CPERSON_H 
#define CPERSON_H

#include <iostream>
#include <queue> 
#include <map>
#include <tuple>

#include "attack.h"
#include "object.h"
#include "concepts/inventory.h"
#include "actions/dialog.h"
#include "item.h"
#include "tools/fuzzy.h"
#include "tools/webcmd.h"

using std::string;

class CPerson : public CObject {
  protected:

    ///Additional descriptions
    CText* m_roomDescription;           //< Player in room, sometimes as mind.
    CText* m_sShotDescription;          //< Brief description, never as mind.
    nlohmann::json m_deadDescription;   //< Description for when the character is dead.
		std::string color_;

    bool will_faint_; ///< Indicates whether this char will faint or die
    bool fainted_;   ///< 
    
    ///Stats, such as live points, strength, virtue etc.
    typedef map<string, int> stats;
    stats attributes_;

    ///This Persons attacks
    typedef std::map<string, CAttack*> attacks;
    attacks m_attacks;  

    ///Inventory of this person
    CInventory m_inventory;                     

    ///Persons dialogue
    CDialog* m_dialog;
    
    ///Map dialogs
    std::map<std::string, CDialog*> m_dialogs;

  public:

    /**
    * Constructor for Person, i.e. "character". Calls constructor from base class CObject.
    * param[in] jAtts json with attributes
    * param[in] dialogue dialogue of this character.
    * param[in] items list of items 
    * param[in] attacks list of attacks
    */
    typedef std::map<std::string, CItem*> map_type;
    CPerson(nlohmann::json jAttributes, CDialog* dialog, attacks newAttacks, CText* text, CPlayer* p,
        std::map<std::string, CDialog*> dialogs, map_type=map_type());

    // *** GETTER *** // 

    ///return map of stats.
    std::map<std::string, int>& getStats();

    /**
    * Return person's stats, like strength, virtue, live-points, by passing name of stat.
    * @param[in] id specify which stat shall be returned.
    * @return return given stat.
    */
    int getStat(std::string id, int default_value=999);

    /// return whether character can faint or dies immediately 
    bool will_faint();
		
    /// return whether character is currently fainted 
		bool fainted();

		std::string color() const;

    ///Return person's attacks.
    attacks& getAttacks();
    std::map<std::string, std::string> getAttacks2();

    ///Return person's inventory.
    CInventory& getInventory();

    ///Return person's dialogue.
    CDialog* getDialog();

    ///Return dialog from map
    CDialog* getDialog(std::string dialog);

    ///Description where the player is located in the room
    std::string getRoomDescription();    

    ///Get description of character, in non-spoken format.
    std::string getReducedDescription();

    ///Get description of character, when he is dead.
    nlohmann::json getDeadDescription();


    // *** SETTER *** //

    /// Set a new stat of this person
    void setStat(std::string, int stat);

    /// Set person's dialogue.
    void setDialog(CDialog* newDialog);

    /// Set person's dialogue, taking a dialog from persons map of dialogues.
    void setDialog(std::string dialog);

		/// Set whether char is currently fainted.
		void setFainted(bool fainted);


    // *** Attacks *** // 

    /**
    * Print all attacks. Attacks are printed in the form: Name \n Strengt\n Description.
    */
    string printAttacks();
    string printAttacksFight();
    string getAttack(string sPlayerChoice);


    // *** Various functions *** //

    /**
    * Check if a given attribute exists.
    * @param[in] name of attribute.
    * @return whether attribute exists or not.
    */
    bool attributeExists(std::string sAttribute);

    std::string getAllInformation();


    // *** Functions needed in CPlayer *** //
    virtual void throw_events(std::string, std::string) { std::cout << "FATAL!!!\n"; }
    virtual void setStatus(string)   { std::cout << "FATAL!!!\n"; }
    virtual void appendPrint(string, bool dont_throw_post=false) { std::cout << "FATAL!!!\n"; }
    virtual void appendSuccPrint(string) { std::cout << "FATAL!!!\n"; }
};

#endif

        
