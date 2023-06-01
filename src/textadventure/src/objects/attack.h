#ifndef CATTACK_H
#define CATTACK_H

#include "objects/helper/update.h"
#include <iostream>
#include <map>

using std::string;

class CAttack
{
private:
    string m_sName;
    string m_sDescription;
    string m_sOutput;
    size_t m_power;
		Updates enemy_update_;
		Updates player_update_;

public:
    CAttack(string sName, string sDescription, string sOutput, const nlohmann::json& json);

    // *** GETTER *** // 
    string getName() const;
    string getDescription() const;
    string getOutput() const;
		const Updates& enemy_update() const;
		const Updates& player_update() const;
};

#endif

 
