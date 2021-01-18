#include "area.h"
#include "nlohmann/json.hpp"
#include "page/category.h"
#include "util/error_codes.h"
#include "util/func.h"
#include <iostream>
#include <string>

Area::Area(std::string base_path, std::string path, nlohmann::json objects) 
  : Page(base_path, path) {
  objects_ = objects;
  UpdateNodes();
  category_ = (--parent_nodes_.rbegin())->second;
}

ErrorCodes Area::AddElem(std::string path, std::string name) {
  std::cout << "Area::AddElem(" << path << ")" << std::endl;
  // If path was object-path, then no new object can be created.
  if (IsObject(path))
    return ErrorCodes::PATH_NOT_FOUND;
  // Check if element already exists.
  if (objects_.count(name) > 0) 
    return ErrorCodes::ALREADY_EXISTS;
  // Create new element from template, add to elements and write to disc.
  objects_[name] = GetObjectFromTemplate(name);
  func::WriteJsonToDisc(path_ + ".json", objects_);
  return ErrorCodes::SUCCESS; 
}

ErrorCodes Area::DelElem(std::string path, std::string name) {
  std::cout << "Area::DelElem(" << path << ")" << std::endl;
  // If path was object-path, then no new object can be created.
  if (IsObject(path))
    return ErrorCodes::PATH_NOT_FOUND;
  if (objects_.count(name) == 0) 
    return ErrorCodes::PATH_NOT_FOUND;
  // Erase element from elements and write to disc.
  last_deleted_obj = objects_[name];
  objects_.erase(name);
  func::WriteJsonToDisc(path_ + ".json", objects_);
  return ErrorCodes::SUCCESS; 
}

ErrorCodes Area::UndoDelElem() {
  std::cout << "Area::UndoDelElem()" << std::endl;
  if (last_deleted_obj.empty() || !(last_deleted_obj.count("id") > 0))
    return ErrorCodes::FAILED;
  objects_[last_deleted_obj["id"].get<std::string>()] = last_deleted_obj;
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

void Area::UpdateNodes() {
  for (auto it=objects_.begin(); it!=objects_.end(); it++)
    child_nodes_[path_.substr(base_path_.length()) + "/" + it.key()] = it.key();
}

bool Area::IsObject(std::string path) { 
  return (path_ != path); 
}
