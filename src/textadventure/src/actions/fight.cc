#include "fight.h"
#include "objects/player.h"
#include "tools/func.h"
#include <algorithm>
#include <cctype>

CFight::CFight(CPlayer* player, std::vector<CPerson*> opponents) {
	m_sName = "Fight";

	std::map<std::string, int> names;
	for (const auto& it : opponents) {
		std::string name = (names.count(it->name()) > 0) ? it->name() + "-" + std::to_string(names[it->name()]+1) : it->name();
		opponent_names_[it->id()] = name;
		names[it->name()]++;
	}

	m_sDescription = "Fighting against: ";
	std::cout << "Starting fight agains: ";
	for (const auto& it : opponents)  {
		m_sDescription += it->name() + ", ";
		std::cout << it->id() << ", " << std::endl;
	}
	m_sDescription.pop_back();
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
	auto [attacker, attacker_name] = GetAttacker();
	auto const& [target, attack_id] = GetTargetAndAttack(inp);
	if (!target) 
		std::cout << "CFight::NextTurn: failed." << std::endl;
	else {
		cur_ = (cur_ == static_cast<int>(opponents_.size()-1)) ? -1 : cur_+1;  // update current
		CAttack* attack = attacker->getAttacks().at(attack_id);
		msg += attacker_name + " " + player_->getWorld()->GetSTDText("uses_attack") + " " + attack->getName() + "\n";
		events += attacker->SimpleUpdate(attack->player_update(), msg, player_->getWorld()->attribute_config().attributes_);
		events += target->SimpleUpdate(attack->enemy_update(), msg, player_->getWorld()->attribute_config().attributes_);
		std::cout << "Got events: " << events << std::endl;
		msg += "$ ";
	}
	// Check whether opponentDied and add endFight if no more opponents
	if (events.find("opponentDied") != std::string::npos) {
		if (cur_before == -1) {
			func::Replace(events, "opponentDied", "finishCharacter " + target->id());
			RemoveOpponent(target->id());
			std::cout << "Removed opponent, opponents left: " << opponents_.size() << std::endl;
			if (opponents_.size() == 0)
				events += ";endFight";
		}
	}
	// If fight not ended add next round.
	if (events.find("endFight") == std::string::npos) {
		if (cur_ == -1) 
			msg += "\n" + Print(false);
		else 
			events += ";h_fight {player}";
	}
	player_->appendPrint(msg);
	return events;
}

std::pair<CPerson*, std::string> CFight::GetAttacker() {
	if (cur_ == -1) 
		return {player_, player_->name()};
	return {opponents_[cur_], opponent_names_.at(opponents_[cur_]->id())};
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
	// Try by number 
	if (std::isdigit(name.front())) {
		int num = std::stoi(name)-1;
		if (static_cast<size_t>(num) < opponents_.size())
			return opponents_[num];
	}
	// try by direct match: 
	for (const auto& it : opponents_) {
		if (func::returnToLower(opponent_names_.at(it->id())) == name)
			return it;
	}

	// Try by fuzzy matching
	int min = 1;
	int num = -1;
	for (size_t i=0; i<opponents_.size(); i++) {
		int cur = fuzzy::fuzzy_cmp(opponent_names_.at(opponents_[i]->id()), name);
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
	for (const auto& it : opponents_) {
		std::string name = opponent_names_.at(it->id());
		str += "- " + name + " (" + it->printFightInfos(player_->getWorld()->attribute_config()) + ")\n";
	}
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
   size_t attack = rand() % GetAttacker().first->getAttacks().size() + 1;
   return GetAttacker().first->getAttack(std::to_string(attack));
}

void CFight::RemoveOpponent(std::string id) {
	auto lambda = [id](CPerson* p) { return p->id() == id; };
	auto it = std::find_if(opponents_.begin(), opponents_.end(), lambda);
	if (it != opponents_.end()) {
		std::cout << "opponent " << id << " removed." << std::endl;
		opponents_.erase(it);
	}
	else 
		std::cout << "opponent " << id << " not found." << std::endl;
}
