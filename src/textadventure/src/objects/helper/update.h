#pragma once 

#include "nlohmann/json_fwd.hpp"
#include "tools/calc_enums.h"
#include <map>
#include <string>

struct Update {
	Update() {};
	Update(const nlohmann::json& json);

	// member
	std::string id_;
	calc::ModType mod_type_;
	int value_;

	bool tmp_;
	int secs_;
	int steps_;
	int rounds_;

	// methods
	void ApplyUpdate(int& value) const;
	std::string ToString(bool calc_value_from_steps, bool reverse) const;
};

struct Updates {
	Updates(const nlohmann::json& json);

	const std::vector<Update>& updates() const;
	void Clear();

	// member
	private:
		std::vector<Update> updates_;
};
