#ifndef CPERSON_H 
#define CPERSON_H

#include <iostream>
#include <queue> 
#include <map>
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
    int m_hp;
    size_t m_strength;
    int m_gold;

    typedef std::map<string, CAttack*> attacks;
    attacks m_attacks;

    //Dialog
    SDialog* m_dialog;

public:
    
    virtual ~CPerson() {}

    // *** GETTER *** // 
    string getName();
    string getID();
    int getHp();
    size_t getStrength();
    int getGold();
    SDialog* getDialog();
    attacks& getAttacks();

    // *** SETTER *** //
    void setDialog(SDialog* newDialog);
    void setHp(int hp);

    string printAttacks();
    string getAttack(string sPlayerChoice);

    // *** Functions needed in CPlayer *** //
    virtual void addItem(CItem*)     { std::cout << "FATAL!!!\n"; }
    virtual void setStatus(string)   { std::cout << "FATAL!!!\n"; }
    virtual void appendPrint(string) { std::cout << "FATAL!!!\n"; }
};

#endif

        
