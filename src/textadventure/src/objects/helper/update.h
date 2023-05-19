#pragma once 

#include "nlohmann/json_fwd.hpp"
#include "tools/calc_enums.h"
#include <map>
#include <string>

struct Update {
	Update() {};
	Update(const nlohmann::json& json);
	Update(const Update& update);
	Update(std::string attribute, double value, calc::ModType mod_type);

	// member
	std::string id_;
	calc::ModType mod_type_;
	double value_;
  std::string attribute_;

	bool tmp_;
	int secs_;
	int steps_;
	int rounds_;

	// methods
	void ApplyUpdate(int& value, int use_value) const;
	std::string ToString(bool calc_value_from_steps, bool reverse) const;
  void ReverseModType();
};

struct Updates {
	Updates(const nlohmann::json& json);

  /** 
   * Generates from list of updates. Used when calling Reverse().
   */
  Updates(const std::vector<Update>& updates);

  /**
   * Generates simple updates (non-temporary, no attribute). Used f.e. for item
   * costs.
   */
	Updates(std::map<std::string, int> attributes, calc::ModType mod_type);

	const std::vector<Update>& updates() const;
	void Clear();

  /** 
   * Create new Updates object completely reversed.
   */
  Updates Reverse() const;

	// member
	private:
		std::vector<Update> updates_;
};
