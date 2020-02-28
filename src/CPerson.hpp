#ifndef CPERSON_H 
#define CPERSON_H

#include <iostream>
#include <queue> 
#include <map>
#include <tuple>
#include "CInventory.hpp"
#include "CItem.hpp"
#include "CDialog.hpp"
#include "CAttack.hpp"
#include "CItem.hpp"
#include "fuzzy.hpp"
#include "Webcmd.hpp"

using std::string;

class CPerson
{
protected:
    string m_sName;
    string m_sID;

    //Stats
    typedef map<string, int> stats;
    stats m_stats;

    CInventory m_inventory;

    typedef std::map<string, CAttack*> attacks;
    attacks m_attacks;

    //Dialog
    SDialog* m_dialog;

public:
    
    virtual ~CPerson() {}

    // *** GETTER *** // 
    string getName();
    string getID();
    int getStat(std::string id);
    SDialog* getDialog();
    attacks& getAttacks();
    CInventory& getInventory();

    // *** SETTER *** //
    void setDialog(SDialog* newDialog);
    void setStat(std::string, int stat);

    // *** Attacks *** // 
    string printAttacks();
    string getAttack(string sPlayerChoice);

    // *** Functions needed in CPlayer *** //
    virtual void throw_event(std::string) {}
    virtual void setStatus(string)   { std::cout << "FATAL!!!\n"; }
    virtual void appendPrint(string) { std::cout << "FATAL!!!\n"; }

};

#endif

        
