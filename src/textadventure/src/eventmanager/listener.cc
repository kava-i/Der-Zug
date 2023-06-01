#include "listener.h"
#include "nlohmann/json_fwd.hpp"
#include "tools/fuzzy.h"
#include <set>
#include <stdexcept>
#include <string>
#include <strings.h>

#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

const std::set<std::string> CListener::fuzzy_event_types = {"pick", "go", "look", "search",
  "talk", "consume", "read", "equipe", "examine"};

// constructors


CListener::CListener(std::string id, std::string listener, std::string location, std::string location_id,
    std::string target_identifier, std::string new_identifier, int permeable, bool self_delete,
    int priority, std::string str_event_type) 
  : id_(id), handler_(listener), location_(location), location_id_(location_id), 
  target_identifier_(target_identifier), new_identifier_(new_identifier), permeable_(permeable),
  self_delete_(self_delete), priority_(priority)
{
  event_type_str_ = str_event_type;
  m_check_function = &CListener::StringCompare;

}

CListener::CListener(std::string id, std::string listener, std::string location, std::string location_id,
    std::string target_identifier, std::string new_identifier, int permeable, bool self_delete,
    int priority, std::regex regex_event_type, int pos) 
  : id_(id), handler_(listener), location_(location), location_id_(location_id), 
  target_identifier_(target_identifier), new_identifier_(new_identifier), permeable_(permeable),
  self_delete_(self_delete), priority_(priority)
{
  event_type_regex_ = regex_event_type;
  m_check_function = &CListener::RegexCompare;
  pos_ = pos;
}

CListener::CListener(std::string id, std::string listener, std::string location, std::string location_id,
    std::string target_identifier, std::string new_identifier, int permeable, bool self_delete,
    int priority, std::vector<std::string> array_event_type) 
  : id_(id), handler_(listener), location_(location), location_id_(location_id), 
  target_identifier_(target_identifier), new_identifier_(new_identifier), permeable_(permeable),
  self_delete_(self_delete), priority_(priority)
{
  event_type_array_ = array_event_type;
  m_check_function = &CListener::InVector;
}

CListener::CListener(std::string id, std::string listener, std::string location, std::string location_id,
    std::string target_identifier, std::string new_identifier, int permeable, bool self_delete,
    int priority, std::map<std::string, std::string> map_event_type)
  : id_(id), handler_(listener), location_(location), location_id_(location_id), 
  target_identifier_(target_identifier), new_identifier_(new_identifier), permeable_(permeable),
  self_delete_(self_delete), priority_(priority)
{
  event_type_map_ = map_event_type;
  m_check_function = &CListener::InMap;
}

CListener* CListener::FromJson(nlohmann::json it, std::string location, std::string location_id) {
  std::cout << "Creating Listener" << std::endl;
  CListener* listener;
  try {
		// use <location_id>_<handler> as default id.
		std::string id = (it.contains("id") && it["id"] != "") 
			? it["id"].get<std::string>() : location_id + "_" + it["handler"].get<std::string>();
    // Regex listener
    if (it.contains("regex") && it["regex"] == true) {
      listener = new CListener(id, it["handler"], location, location_id, 
          it.value("target", ""), it.value("event_attributes", ""), it.value("permeable", -1), 
          it.value("delete", 0), it.value("priority", 0), (std::regex)it["cmd"], it["pos"]);
    }
    // String listener
    else {
      listener = new CListener(id, it["handler"], location, location_id, 
          it.value("target", ""), it.value("event_attributes", ""), it.value("permeable", -1), 
          it.value("delete", 0), it.value("priority", 0), (std::string)it["cmd"]);
    }
  }
  catch (std::exception& e) {
    std::cout << "Failed to create handler" << it.dump() << std::endl;
    throw std::runtime_error("Failined creating listener at " + location_id + ": " + it.dump() + ": " + e.what());
  }
  return listener;
}

// getter
std::string CListener::id() {
  return id_;
}
std::string CListener::handler() {
  return handler_;
}
int CListener::permeable() {
  return permeable_;
}
bool CListener::self_delete() {
  return self_delete_;
}
int CListener::priority() {
  return priority_;
}
std::string CListener::location() {
  return location_;
}
std::string CListener::location_id() {
  return location_id_;
}
std::string CListener::new_identifier() {
  return new_identifier_;
}

// functions

bool CListener::check_match(event& e) {
  bool match = (this->*m_check_function)(e);
	if (new_identifier_ != "") {
		e.second = new_identifier_;
	}
	return match;
}

bool CListener::StringCompare(event& e) {
  // Check event type, if not equal, retuirn false.
  if (e.first != event_type_str_)
    return false;
  // If target is specified, also checkCheck event target.
  if (target_identifier_ != "") {
		std::cout << "StringCompare: " << e.first << "|" << e.second << " ~ " << target_identifier_ << std::endl;
		std::cout << "StringCompare: " << (fuzzy_event_types.count(e.first) > 0 && 
				fuzzy::fuzzy_cmp(e.second, target_identifier_) > 0.2) << std::endl;
		std::cout << "StringCompare: " << (e.second != target_identifier_) << std::endl;
    // If match, update identifier.
    if ((fuzzy_event_types.count(e.first) > 0 || fuzzy::fuzzy_cmp(e.second, target_identifier_) > 0.2)
        && e.second != target_identifier_)
      return false;
  }
  return true;
}

bool CListener::RegexCompare(event& e) {
  std::string str = e.first + " " + e.second;
  std::smatch m;
  if (std::regex_match(str, m, event_type_regex_)) {
    if(e.first != e.second)
      e.second = m[pos_];
    return true;
  }
  return false;
}

bool CListener::InVector(event& e) {
	std::cout << "InVector: " << e.first << std::endl;
  try {
    unsigned int index = std::stoi(e.first);
		// allow to be one greater that the size of the last index to allow 1
		// initialzed player input, while vector is 1 initialzed.
    if (index > 0 && index <= event_type_array_.size()) {
			std::cout << "passed on index: " << index << std::endl;
      return true;
		}
  } catch (...) {}

  return func::inArray(event_type_array_, e.first);
}

bool CListener::InMap(event& e) {
  return func::getObjectId(event_type_map_, e.first) != "";
}
