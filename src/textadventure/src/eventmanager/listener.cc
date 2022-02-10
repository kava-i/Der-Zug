#include "listener.h"
#include <string>

// constructors
CListener::CListener(std::string id, std::string event_type) {
  id_ = id;
  event_type_str_ = event_type;
  m_check_function = &CListener::StringCompare;
}
    
CListener::CListener(std::string id, std::regex event_type, int pos) {
  id_ = id;
  event_type_regex_ = event_type;
  m_pos = pos;
  m_check_function = &CListener::RegexCompare;
}

CListener::CListener(std::string id, std::vector<std::string> event_type) {
  id_ = id;
  event_type_array_ = event_type;
  m_check_function = &CListener::InVector;
}

CListener::CListener(std::string id, std::map<std::string, std::string> event_type) {
  id_ = id;
  event_type_map_ = event_type;
  m_check_function = &CListener::InMap;
}

// getter

std::string CListener::id() {
  return id_;
}


// functions

bool CListener::check_match(event& e) {
  return (this->*m_check_function)(e);
}

bool CListener::StringCompare(event& e) {
  return e.first == event_type_str_;
}

bool CListener::RegexCompare(event& e) {
  std::string str = e.first + " " + e.second;
  std::smatch m;
  if (std::regex_match(str, m, event_type_regex_)) {
    if(e.first != e.second)
      e.second = m[m_pos];
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
