#ifndef CPERSON_H 
#define CPERSON_H

#include <iostream>
#include <queue> 
#include <map>
#include <tuple>
#include "CObject.hpp"
#include "CInventory.hpp"
#include "CItem.hpp"
#include "CDialog.hpp"
#include "CAttack.hpp"
#include "CItem.hpp"
#include "fuzzy.hpp"
#include "Webcmd.hpp"

using std::string;

class CPerson : public CObject
{
protected:

    ///Stats, such as live points, strength, virtue etc.
    typedef map<string, int> stats;
    stats m_stats;

    ///This Persons attacks
    typedef std::map<string, CAttack*> attacks;
    attacks m_attacks;  

    ///Inventory of this person
    CInventory m_inventory;                     

    ///Persons dialogue
    CDialog* m_dialog;

public:

    /**
    * Constructor for Person, i.e. "character". Calls constructor from base class CObject.
    * param[in] jAtts json with attributes
    * param[in] dialogue dialogue of this character.
    * param[in] items list of items 
    * param[in] attacks list of attacks
    */
    typedef std::map<std::string, CItem*> map_type;
    CPerson(nlohmann::json jAttributes, CDialog* dialog, attacks newAttacks, CPlayer* p, map_type=map_type());

    // *** GETTER *** // 

    /**
    * Return person's stats, like strength, virtue, live-points, by passing name of stat.
    * @param[in] id specify which stat shall be returned.
    * @return return given stat.
    */
    int getStat(std::string id);

    ///Return person's attacks.
    attacks& getAttacks();
    std::map<std::string, std::string> getAttacks2();

    ///Return person's inventory.
    CInventory& getInventory();

    ///Return person's dialogue.
    CDialog* getDialog();

    
    // *** SETTER *** //

    ///Set a new stat of this person
    void setStat(std::string, int stat);

    ///Set person's dialogue.
    void setDialog(CDialog* newDialog);


    // *** Attacks *** // 

    /**
    * Print all attacks. Attacks are printed in the form: Name \n Strengt\n Description.
    */
    string printAttacks();
    string getAttack(string sPlayerChoice);


    // *** Various functions *** //

    /**
    * Check if a given attribute exists.
    * @param[in] name of attribute.
    * @return whether attribute exists or not.
    */
    bool attributeExists(std::string sAttribute);


    // *** Functions needed in CPlayer *** //
    virtual void throw_event(std::string) { std::cout << "FATAL!!!\n"; }
    virtual void setStatus(string)   { std::cout << "FATAL!!!\n"; }
    virtual void appendPrint(string) { std::cout << "FATAL!!!\n"; }
    virtual void appendSuccPrint(string) { std::cout << "FATAL!!!\n"; }
    virtual void addEP(int ep)       { std::cout << "FATAL!!!\n"; }
};

#endif

        
