#include "fight.h"
#include "objects/player.h"
#include "tools/func.h"
#include <cctype>

CFight::CFight(CPlayer* player, std::vector<CPerson*> opponents) {
	m_sName = "Fight";
	m_sDescription = "Fighting against: ";
	for (const auto& it : opponents) 
		m_sDescription += it->name() + ",";
	m_sDescription.pop_back();
  player_ = player;
  opponents_= opponents;
	cur_ = -1;
}

CPerson* CFight::getOpponent() { 
	return opponents_.front();
}

void CFight::InitializeFight() {
  player_->appendPrint(Print(true));
}

std::string CFight::NextTurn(const std::string& inp) {
	std::string events;
	std::string msg;
	int cur_before = cur_;
	auto attacker = GetAttacker();
	auto const& [target, attack_id] = GetTargetAndAttack(inp);
	if (!target) 
		std::cout << "CFight::NextTurn: failed." << std::endl;
	else {
		cur_ = (cur_ == static_cast<int>(opponents_.size()-1)) ? -1 : cur_+1;  // update current
		CAttack* attack = attacker->getAttacks().at(attack_id);
		msg += attacker->name() + " " + player_->getWorld()->GetSTDText("uses_attack") + " " + attack->getName() + "\n";
		events += attacker->SimpleUpdate(attack->player_update(), msg, player_->getWorld()->attribute_config().attributes_);
		events += target->SimpleUpdate(attack->enemy_update(), msg, player_->getWorld()->attribute_config().attributes_);
		std::cout << "Got events: " << events << std::endl;
		msg += "$ ";
	}
	// If fight not ended add next round.
	if (events.find("endFight") == std::string::npos) {
		if (cur_ == -1) 
			msg += "\n" + Print(false);
		else 
			events += ";h_fight {player}";
	}
	else {
		if (cur_before == -1) {
			events += ";finishCharacter " + target->id();
		}
	}
	player_->appendPrint(msg);
	return events;
}

CPerson* CFight::GetAttacker() {
	return (cur_ == -1) ? player_ : opponents_[cur_];
}

std::pair<CPerson*, std::string> CFight::GetTargetAndAttack(const std::string& inp) {
	if (inp == "{player}") {
		return {player_, pickOpponentAttack()};
	}
	if (inp.find("|") == std::string::npos) {
		std::cout << "CFight::NextTurn: invalid input, seperate like this: \"attack|opponent\";" << std::endl;
		return {nullptr, ""};
	}
	std::string attack_name = func::split(inp, "|")[0];
	std::string target_name = func::split(inp, "|")[1];
	std::string attack_id = player_->getAttacksID(attack_name);
	if (attack_id == "") {
		std::cout << "CFight::NextTurn: attack not found: " << attack_name << std::endl;
		return {nullptr, ""};
	}
	return {GetTarget(target_name), attack_id};
}

CPerson* CFight::GetTarget(std::string name) {
	if (name == "{player}") 
		return player_;
	if (std::isdigit(name.front())) {
		int num = std::stoi(name)-1;
		if (num < opponents_.size())
			return opponents_[num];
	}
	int min = 1;
	int num = -1;
	for (int i=0; i<opponents_.size(); i++) {
		int cur = fuzzy::fuzzy_cmp(opponents_[i]->name(), name);
    if (cur <= 0.2 && cur < min) {
			min = cur;
			num = i;
		}
	}
	if (num != -1)
		return opponents_[num];
	return nullptr;
}

std::string CFight::Print(bool add_help) {
	// TODO (fux): print attributes selected in config
	std::string str = player_->getWorld()->GetSTDText("fighting_against") + "\n";
	for (const auto& it : opponents_) 
		str += "- " + it->name() + " (" + it->printFightInfos(player_->getWorld()->attribute_config()) + ")\n";
	str += player_->name() + ": " + player_->printFightInfos(player_->getWorld()->attribute_config());
	if (add_help) {
		str += "\n" + player_->printAttacks();
		str += player_->getWorld()->GetSTDText("fight_help");
	}
	str += "\n";
	return str;
}

string CFight::pickOpponentAttack() {
   srand(time(NULL));
   size_t attack = rand() % GetAttacker()->getAttacks().size() + 1;
   return GetAttacker()->getAttack(std::to_string(attack));
}
