#include "area.h"
#include "nlohmann/json.hpp"
#include "page/category.h"
#include "util/error_codes.h"
#include "util/func.h"
#include <cstddef>
#include <exception>
#include <iostream>
#include <string>

Area::Area(std::string base_path, std::string path, nlohmann::json objects) 
  : Page(base_path, path) {
  objects_ = objects;
  UpdateNodes();
  category_ = (--parent_nodes_.rbegin())->second;
}

std::string Area::category() const { 
  return category_; 
}

ErrorCodes Area::ModifyObject(std::string path, std::string name, nlohmann::json modified_object) {
  std::cout << "Area::ModifyObject(" << path << ")" << std::endl;
  // If path was object-path, or object does not exist, object cannot be modified.
  if (objects_.count(name) == 0)
    return ErrorCodes::PATH_NOT_FOUND;

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

ErrorCodes Area::RestoreBackupObj() {
  std::cout << "Area::UndoDelElem()" << std::endl;
  if (backup_obj_.empty() || !(backup_obj_.count("id") > 0))
    return ErrorCodes::FAILED;
  objects_[backup_obj_["id"].get<std::string>()] = backup_obj_;
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
  if (objects_.count(path.substr(path.rfind("/")+1)) == 0)
    return nlohmann::json({{"error", "Problem parsing json"}});
  nlohmann::json object = objects_[path.substr(path.rfind("/")+1)];
  object["__parents"] = parent_nodes_;
  nlohmann::json json = nlohmann::json({{"header", object}, 
      {"path", "web/object_templates/" + category_ + ".html"}});
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
    std::string name = path.substr(path.rfind("/")+1);
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

bool Area::IsObject(std::string path) { 
  return (path_ != path); 
}
