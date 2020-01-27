#ifndef CFIGHT_H
#define CFIGHT_H

#include <iostream>
#include <map>
#include "CPerson.hpp"
#include "CAttack.hpp"
#include "CContext.hpp"

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

    string printStats(CPerson* person);
    string createFightingAgainst();
    string pickOpponentAttack();
};

#endif

 
