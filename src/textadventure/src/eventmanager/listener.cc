#include "listener.h"
#include "nlohmann/json_fwd.hpp"
#include <string>

#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

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
  try {
    // Regex listener
    if (it.contains("regex") && it["regex"] == true) {
      return new CListener(it["id"], it["handler"], location, location_id, 
          it.value("target", ""), it.value("event_attributes", ""), it.value("permeable", -1), 
          it.value("delete", false), it.value("priority", 0), (std::regex)it["cmd"], it["pos"]);
    }
    // String listener
    else {
      return new CListener(it["id"], it["handler"], location, location_id, 
          it.value("target", ""), it.value("event_attributes", ""), it.value("permeable", -1), 
          it.value("delete", false), it.value("priority", 0), (std::string)it["cmd"]);
    }
  }
  catch (std::exception& e) {
    throw("Failined creating listener: " + it.dump() + ": " + e.what());
  }
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

// functions

bool CListener::check_match(event& e) {
  return (this->*m_check_function)(e);
}

bool CListener::StringCompare(event& e) {
  if (e.first != event_type_str_)
    return false;
  if (target_identifier_ != "" && e.second == target_identifier_)
    e.second = new_identifier_;
  else if (target_identifier_ != "" && e.second != target_identifier_)
    return false;
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
  try {
    unsigned int index = std::stoi(e.first);
    if (index > 0 && index < event_type_array_.size())
      return true;
  } catch (...) {}

  return func::inArray(event_type_array_, e.first);
}

bool CListener::InMap(event& e) {
  return func::getObjectId(event_type_map_, e.first) != "";
}
