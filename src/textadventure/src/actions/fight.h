#ifndef CFIGHT_H
#define CFIGHT_H

#include <iostream>
#include <map>
#include "concepts/quest.h"
#include "objects/person.h"
#include "objects/attack.h"

using std::string;

class CFight
{
protected:
    string m_sName;
    string m_sDescription;

		CPlayer* player_;
		std::vector<CPerson*> opponents_;
		int cur_;

public:

    CFight(CPlayer* player, std::vector<CPerson*> opponents);

		// getter 
    CPerson* getOpponent();

		// methods
    void InitializeFight();
		std::string NextTurn(const std::string& inp);
    string pickOpponentAttack();

private:
		/**
		 * Given either a number (n) or a name, returns the nth opponent or the
		 * opponent that matches the given name sufficiently. In case of "{player}"
		 * given, always returns the player.
		 */
		CPerson* GetTarget(std::string id);
		CPerson* GetAttacker();

		std::pair<CPerson*, std::string> GetTargetAndAttack(const std::string& inp);
		std::string Print(bool add_help);

};

#endif

 
