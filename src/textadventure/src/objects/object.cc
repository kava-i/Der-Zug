#include "objects/object.h"
#include "eventmanager/listener.h"

CObject::CObject(nlohmann::json jAttributes, CPlayer* p, std::string location) {
  id_ = jAttributes.value("id", "");
  name_ = jAttributes.value("name", "");
  text_ = new CText(jAttributes.value("description", nlohmann::json::parse("{}") ), p);
  // Create listeners.
  for (const auto& it : jAttributes.value("listeners", std::vector<nlohmann::json>())) {
		auto new_listener = CListener::FromJson(it, location, id_);
		listeners_[new_listener->id()] = new_listener;
  }
  image_ = jAttributes.value("image", "");
  music_ = jAttributes.value("music", "");
}

std::string CObject::id() {
  return id_;
}

///Return name of object.
std::string CObject::name() {
  return name_;
}

///Return description by calling print-function from text class.
std::string CObject::text() {
  return text_->print();
}

///Return list of handler
std::vector<CListener*> CObject::listeners() {
  std::vector<CListener*> listeners;
  for (const auto& it : listeners_)
    listeners.push_back(it.second);
  return listeners;
}

std::string CObject::image() {
  return image_;
}

std::string CObject::music() {
  return music_;
}

///Set id of object.
void CObject::set_id(std::string sID) {
    id_ = sID;
}

///Set name of object.
void CObject::set_name(std::string sName) {
    name_ = sName;
}

/// set media.
void CObject::set_image(std::string file_name) {
  image_ = file_name;
}

/// set new music.
void CObject::set_music(std::string file_name) {
  music_ = file_name;
}

///Removes handler:
void CObject::RemoveHandler(std::string key) {
  if (listeners_.count(key) > 0) {
    delete listeners_[key];
    listeners_.erase(key);
    std::cout << key << " removed." << std::endl;
    return;
  }
  else {
    std::cout << cRED << "Object::RemoveHandler: listener " << key << " does not exist!" 
      << std::endl;
  }
}
