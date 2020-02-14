#ifndef CPERSON_H 
#define CPERSON_H

#include <iostream>
#include <queue> 
#include <map>
#include <tuple>
#include "CDialog.hpp"
#include "CAttack.hpp"
#include "CItem.hpp"
#include "fuzzy.hpp"

using std::string;

class CPerson
{
protected:
    string m_sName;
    string m_sID;

    //Stats
    typedef map<string, int> stats;
    stats m_stats;

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

    // *** SETTER *** //
    void setDialog(SDialog* newDialog);
    void setStat(std::string, int stat);

    string printAttacks();
    string getAttack(string sPlayerChoice);

    // *** Functions needed in CPlayer *** //
    virtual void throw_event(std::string) {}
    virtual void addItem(CItem*)     { std::cout << "FATAL!!!\n"; }
    virtual void setStatus(string)   { std::cout << "FATAL!!!\n"; }
    virtual void appendPrint(string) { std::cout << "FATAL!!!\n"; }
};

#endif

        
