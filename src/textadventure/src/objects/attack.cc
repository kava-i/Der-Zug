#include "attack.h"
#include "objects/helper/update.h"
#include "nlohmann/json.hpp"

CAttack::CAttack(string sName, string sDescription, string sOutput, const nlohmann::json& json) 
	: enemy_update_(Updates(json.value("updates", nlohmann::json::array()))), 
		player_update_(Updates(json.value("player_updates", nlohmann::json::array()))) 
{
	m_sName = sName;
	m_sDescription = sDescription;
	m_sOutput = sOutput;
}

// *** GETTER *** //

string CAttack::getName() const { 
	return m_sName;
}
string CAttack::getDescription() const { 
	return m_sDescription; 
}
string CAttack::getOutput() const { 
	return m_sOutput; 
}
const Updates& CAttack::enemy_update() const {
	return enemy_update_;
}
const Updates& CAttack::player_update() const {
	return player_update_;
}

