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

    void InitializeFight();
    string FightRound(string sPlayerChoice);
    string Turn(string selectedAttack, CPerson* attacker, CPerson* defender);

    string PrintStats(CPerson* person,bool printDesc=true);
    string createFightingAgainst();
    string pickOpponentAttack();
};

#endif

 
