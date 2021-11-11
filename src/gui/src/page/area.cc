#include "area.h"
#include "nlohmann/json.hpp"
#include "page/category.h"
#include "util/error_codes.h"
#include "util/func.h"
#include <cstddef>
#include <exception>
#include <iostream>
#include <math.h>
#include <string>

Area::Area(std::string base_path, std::string path, nlohmann::json objects) 
  : Page(base_path, path) {
  objects_ = objects;
  UpdateNodes();
  category_ = (--parent_nodes_.rbegin())->second;
  std::ifstream read(base_path_ + "/" + path_ + "/" + name_ + ".md");
  if (read) {
    read >> notes_;
    read.close();
  }
}

std::string Area::category() const { 
  return category_; 
}

std::string Area::notes(std::string path) const {
  if (IsObject(path)) {
    if (obj_notes_.contains(func::GetLastElemFromPath(path)))
      return obj_notes_.at(func::GetLastElemFromPath(path)); 
    return "";
  }
  std::ofstream write(base_path_ + "/" + path_ + "/" + name_ + ".md");
  write << notes_;
  write.close();
  return notes_;
}

void Area::set_notes(std::string path, std::string notes) {
  if (IsObject(path)) {
    if (obj_notes_.contains(func::GetLastElemFromPath(path)))
      obj_notes_[func::GetLastElemFromPath(path)] = notes;
  }
  notes_ = notes;
}

ErrorCodes Area::ModifyObject(std::string path, std::string name, nlohmann::json modified_object) {
  std::cout << "Area::ModifyObject(" << path << ")" << std::endl;
  // If path was object-path, or object does not exist, object cannot be modified.
  if (objects_.count(name) == 0) {
    name = path.substr(path.rfind("/")+1);

    if (objects_.count(name) == 0) {
      std::cout << "Object not found with name: " << name << std::endl;
      std::cout << objects_ << std::endl;
      return ErrorCodes::PATH_NOT_FOUND;
    }
  }

  // Create a backup of the original object, then overwrite original with modified object.
  backup_obj_ = objects_[name];
  objects_[name] = modified_object; 
  func::WriteJsonToDisc(path_ + ".json", objects_);
  return ErrorCodes::SUCCESS; 
}

ErrorCodes Area::AddElem(std::string path, std::string name, nlohmann::json infos) {
  std::cout << "Area::AddElem(" << path << ")" << std::endl;
  // If path was object-path, then no new object can be created.
  nlohmann::json new_obj;
  if (IsObject(path) || !GetObjectFromTemplate(new_obj, name))
    return ErrorCodes::PATH_NOT_FOUND;
  // Check if element already exists.
  if (objects_.count(name) > 0) 
    return ErrorCodes::ALREADY_EXISTS;

  // Join created object from template with given infos add to elements and write to disc.
  std::cout << "new_obj: " << new_obj << std::endl;
  std::cout << "infos: " << infos << std::endl;
  infos =  func::Join(new_obj, infos);
  std::cout << "infos: " << infos << std::endl;
  objects_[name] = infos;
  func::WriteJsonToDisc(path_ + ".json", objects_);
  return ErrorCodes::SUCCESS; 
}

ErrorCodes Area::DelElem(std::string path, std::string name) {
  std::cout << "Area::DelElem(" << path << ")" << std::endl;
  // If path was object-path, or object does not exist, object cannot be deleted.
  if (IsObject(path) || objects_.count(name) == 0)
    return ErrorCodes::PATH_NOT_FOUND;

  // Create backup object, then erase element from elements and write to disc.
  backup_obj_ = objects_[name];
  objects_.erase(name);
  func::WriteJsonToDisc(path_ + ".json", objects_);
  return ErrorCodes::SUCCESS; 
}

ErrorCodes Area::RestoreBackupObj(std::string id) {
  std::cout << "Area::UndoDelElem()" << std::endl;
  if (backup_obj_.empty()) {
    std::cout << "Area::UndoDelElem: failed: no backup object." << std::endl;
    return ErrorCodes::FAILED;
  }
  objects_[id] = backup_obj_;
  func::WriteJsonToDisc(path_ + ".json", objects_);
  return ErrorCodes::SUCCESS;
}

nlohmann::json Area::CreatePageData(std::string path) {
  std::cout << "Area::RenderPage(" << path << ")" << std::endl;
  if (IsObject(path))
    return CreateObjectPageData(path);
  return Page::CreatePageData(path);
}

nlohmann::json Area::CreateObjectPageData(std::string path) {
  std::cout << "Area::RenderObjectPage(" << path << ")" << std::endl;
  if (objects_.count(func::GetLastElemFromPath(path)) == 0)
    return nlohmann::json({{"error", "Problem parsing json"}});
  std::string name = func::GetLastElemFromPath(path);
  nlohmann::json object = objects_[name];
  // If array or no id exists. Convert to object and add id, based on map-key.
  // This is probably due to the object being a section in the config, which is 
  // used slightly differently, but may be usefull, for future stuff.
  if (object.is_array()) {
    object = {{"id", name}, {"content", object}};
  }
  else if (!object.contains("id")) {
    object = {{"id", name}, {"content", object}};
  }
  object["__parents"] = parent_nodes_;
  nlohmann::json json = nlohmann::json({
      {"header", object}, 
      {"path", "web/object_templates/" + category_ + ".html"},
      {"path2", "web/object_templates/" + category_ + "_" + name + ".html"}
    });
  return json;
}

nlohmann::json Area::GetGraph(std::string path) {
  std::cout << "Area::GetGraph(" << path << ")" << std::endl;
  nlohmann::json graph;
  nlohmann::json nodes = nlohmann::json::array(); // nodes
  nlohmann::json links = nlohmann::json::array(); // links

  if (category_ == "rooms") {
    // Iterate over all rooms and nodes and links accordingly.
    for (const auto& room : objects_.items()) {
      // Add node for this room. 
      nodes.push_back({{"id", room.key()}, {"group", "1"}});

      // Check if room has exists.
      if (!room.value().contains("exits"))
        continue;
      // Iterate over exists and add edges
      for (const auto& exit : room.value()["exits"].items()) {
        if (exit.key().find(".") != std::string::npos)
          nodes.push_back({{"id", exit.key()}, {"group", "2"}});
        // If link does not exist yet, add link.
        if (!LinkExists(links, room.key(), exit.key()))
          links.push_back({{"source", room.key()}, {"target", exit.key()}});
      }
    }
  }

  if (category_ == "dialogs") {
    // Iterate over all rooms and nodes and links accordingly.
    int counter = 0;
    for (const auto& state : objects_.items()) {
      // Add node for this room. 
      nodes.push_back({{"id", state.key()}, {"group", std::to_string(counter++)}});

      // Check if room has exists.
      if (!state.value().contains("options"))
        continue;
      // Iterate over exists and add edges
      for (const auto& option : state.value()["options"]) {
        // If link does not exist yet, add link.
        if (!LinkExists(links, state.key(), option["to"]))
          links.push_back({{"source", state.key()}, {"target", option["to"]}});
      }
    }
  }

  if (category_ == "quests" && IsObject(path)) {
    std::string name = func::GetLastElemFromPath(path);
    nlohmann::json quest = objects_[name];

    nodes.push_back({{"id", "start"}, {"group","1"}});
    for (const auto& step : quest["steps"]) {
      nodes.push_back({{"id", step["id"]},{"group","1"}});
     for (const auto& linked: step.value("linkedSteps", nlohmann::json::array()))
      links.push_back({{"source", step["id"]}, {"target", linked}});
    }
    for (const auto& step : quest.value("active_from_beginning", nlohmann::json::array()))
      links.push_back({{"source", "start"}, {"target", step}});
  }

  // If nodes where added, add nodes, and links. This way we asure to pass an
  // empty json, if no nodes exist.
  if (nodes.size() > 0) {
    graph["nodes"] = nodes;
    graph["links"] = links;
  }
  return graph;
}

bool Area::LinkExists(const nlohmann::json& links, std::string source, std::string target) {
  // If no links, the given link does not exist.
  if (links.size() == 0)
    return false;

  // Check if given link exists.
  for (const auto& link : links) {
    // Add graph is undirekt check whether egde in either direction eixts.
    if ((link["source"] == source && link["target"] == target) ||
        (link["target"] == source && link["source"] == target))
      return true;
  }
  return false;
}

void Area::UpdateNodes() {
  child_nodes_.clear();
  for (auto it=objects_.begin(); it!=objects_.end(); it++)
    child_nodes_[path_.substr(base_path_.length()) + "/" + it.key()] = it.key();
}

bool Area::IsObject(std::string path) const { 
  return (path_ != path); 
}
