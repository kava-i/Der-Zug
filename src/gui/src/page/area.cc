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

ErrorCodes Area::AddElem(std::string path, std::string name) {
  std::cout << "Area::AddElem(" << path << ")" << std::endl;
  // If path was object-path, then no new object can be created.
  nlohmann::json new_obj;
  if (IsObject(path) || !GetObjectFromTemplate(new_obj, name))
    return ErrorCodes::PATH_NOT_FOUND;
  // Check if element already exists.
  if (objects_.count(name) > 0) 
    return ErrorCodes::ALREADY_EXISTS;

  // Create new element from template, add to elements and write to disc.
  objects_[name] = new_obj;
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

void Area::UpdateNodes() {
  for (auto it=objects_.begin(); it!=objects_.end(); it++)
    child_nodes_[path_.substr(base_path_.length()) + "/" + it.key()] = it.key();
}

bool Area::IsObject(std::string path) { 
  return (path_ != path); 
}
