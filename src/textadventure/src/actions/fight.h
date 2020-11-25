#ifndef CFIGHT_H
#define CFIGHT_H

#include <iostream>
#include <map>
#include "objects/person.h"
#include "objects/attack.h"

using std::string;

class CFight
{
protected:
    string m_sName;
    string m_sDescription;

    CPerson* m_player;
    CPerson* m_opponent;

public:

    CFight(CPerson* player, CPerson* opponent);

    CPerson* getOpponent();

    void initializeFight();
    string fightRound(string sPlayerChoice);
    string turn(string selectedAttack, CPerson* attacker, CPerson* defender);

    string printStats(CPerson* person,bool printDesc=true);
    string createFightingAgainst();
    string pickOpponentAttack();
};

#endif

 
