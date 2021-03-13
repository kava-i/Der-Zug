#include "world.h"
#include "nlohmann/json.hpp"
#include "page/page.h"
#include "page/sub_category.h"
#include "util/error_codes.h"
#include "util/func.h"
#include <algorithm>
#include <exception>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <shared_mutex>
#include <string>

namespace fs = std::filesystem;

World::World(std::string base_path, std::string path, int port) 
    : base_path_(base_path), path_(path), port_(port) {

  // Parse name and creator from path.
  std::string temp = path.substr(base_path.length()+1);
  name_ = temp.substr(temp.rfind("/")+1);
  creator_ = temp.substr(0, temp.find("/"));

  // Init paths (map of full-path + page-object) and short_paths (vector of url-paths).
  InitializePaths(path_);
  UpdateShortPaths();
}

int World::port() const {
  return port_;
}

ErrorCodes World::ModifyObject(std::string path, std::string id, nlohmann::json modified_obj, bool force) {
  std::cout << "World::ModifyObject" << std::endl;
  std::cout << "World::ModifyObject(" << path << ")" << std::endl;
   // Check that path exists.
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) == 0) 
    return ErrorCodes::PATH_NOT_FOUND;
  
  // Modify object. If not successfull or force-write (thus no need to do further checking), return error-code.
  ErrorCodes error_code = paths_.at(path)->ModifyObject(path, id, modified_obj);
  if (error_code != ErrorCodes::SUCCESS || force) 
    return error_code;
  
  // Make changes permanent if game is still running, revert changes otherwise.
  sl.unlock();
  return RevertIfGameNotRunning(path, id, "modify");
}

ErrorCodes World::AddElem(std::string path, std::string name, bool force) {
  std::cout << "World::AddElem(" << path << ")" << std::endl;
  // Check that path exists.
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) == 0) 
    return ErrorCodes::PATH_NOT_FOUND;
  
  // Add element. If not successfull or force-write (thus no need to do further checking), return error-code.
  ErrorCodes error_code = paths_.at(path)->AddElem(path, func::ConvertToId(name));
  if (error_code != ErrorCodes::SUCCESS || force)
    return error_code;

  // Make changes permanent if game is still running, revert changes otherwise.
  sl.unlock();
  return RevertIfGameNotRunning(path, name, "add");
}

ErrorCodes World::DelElem(std::string path, std::string name, bool force) {
  std::cout << "World::DelElem(" << path << ")" << std::endl;
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) == 0)
    return ErrorCodes::PATH_NOT_FOUND;

  // Delete element. If not successfull or force-write (thus no need to do further checking), return error-code.
  ErrorCodes error_code = paths_.at(path)->DelElem(path, name);
  std::cout << "Checking error-code: " << error_code << ". Force: " << force << std::endl;
  if (error_code != ErrorCodes::SUCCESS || force) 
    return error_code;
 
  // Make changes permanent if game is still running, revert changes otherwise.
  sl.unlock();
  return RevertIfGameNotRunning(path, name, "delete");
}

nlohmann::json World::GetPage(std::string path) const {
  std::cout << "World::GetPage(" << path << ")" << std::endl;
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) == 0)
    return nlohmann::json({{"error", "File not found."}});
  nlohmann::json json = paths_.at(path)->CreatePageData(path);
  json["header"]["__short_paths"] = short_paths_;
  json["header"]["__is_main"] = (path == path_);
  json["header"]["__port"] = port_;
  return json;
}

// paths_
void World::InitializePaths(std::string path) {
  std::cout << "InitializePaths: " << path << std::endl;
  std::unique_lock ul(shared_mtx_paths_);
  //Check if directory is empty.
  if (fs::is_empty(path)) {
    std::cout << "Empty directory." << std::endl;
  }
  // category => elements are directories:
  else if (fs::is_directory(fs::directory_iterator(path)->path())) {
    paths_[path] = new Category(base_path_, path);
    std::cout << "Added Category: " << path << std::endl;
    ul.unlock();
    for (auto& p : fs::directory_iterator(path)) 
      InitializePaths(p.path());
  }
  // subcategories => elements are files (jsons).
  else {
    std::cout << "Adding subcategory: " << path << std::endl;
    paths_[path] = new SubCategory(base_path_, path);
    std::cout << "Added Subcategory: " << path << std::endl;
    for (auto& p : fs::directory_iterator(path)) {
      nlohmann::json objects;
      std::string path_without_extension = func::RemoveExtension(p.path());
      if (p.path().extension() == ".jpg") 
        std::cout << "skipped image" << std::endl;
      else if (!func::LoadJsonFromDisc(p.path(), objects))
        std::cout << "failed to load json." << std::endl;
      else if (objects.is_array() == false) {
        std::cout << "added area" << std::endl;
        paths_[path_without_extension] = new Area(base_path_, path_without_extension, objects);
        // objects: objects in json of objects.
        for (auto it=objects.begin(); it!=objects.end(); it++)
          paths_[path_without_extension + "/" + it.key()] = paths_.at(path_without_extension);
      }
      else
        std::cout << "skiped list type." << std::endl;
    }
  }
  std::cout << "Done initializing paths." << std::endl;
}

void World::UpdateShortPaths() {
  std::shared_lock sl(shared_mtx_paths_);
  for (auto it : paths_)
    short_paths_.push_back(it.first.substr(base_path_.length()));
}

World::~World() {
  std::unique_lock ul(shared_mtx_paths_);
  while (paths_.size() > 0) {
    auto it = paths_.begin();
    std::erase_if(paths_, [&](const auto& elem) { return (elem.second->category() == it->second->name()); });
    delete it->second;
    paths_.erase(it->first);
  }
} 

bool World::IsGameRunning() const {
  //Create command
  std::string command = "./../../textadventure/build/bin/testing.o "
    "--path ../../data/users/" + creator_ + "/files/" + name_ + "/";
  
  //Get players from players.json
  nlohmann::json players;
  if (func::LoadJsonFromDisc(path_ + "/players/players.json", players) == false) 
    return false;

  //Run game with every existing player.
  bool success = true;
  for (auto it=players.begin(); it!=players.end(); it++) {
    std::string test_p = command + " -p " + it.key() +
      " > ../../data/users/" + creator_ + "/logs/" + name_ + "_write.txt";
    if (system(test_p.c_str()) != 0) 
      success = false;
    std::cout << "Running with player \"" << it.key() << "\": " << success << std::endl;
  }
  std::cout << std::endl;
  return success;
}

void World::ConvertId(std::string& id) {
  id = func::ReturnToLower(id);
  std::replace(id.begin(), id.end(), ' ', '_');
}

ErrorCodes World::RevertIfGameNotRunning(std::string path, std::string id, std::string action) {
  if (IsGameRunning()) {
    InitializePaths(path_);
    UpdateShortPaths();
    return ErrorCodes::SUCCESS;
  }
  else {
    std::shared_lock sl(shared_mtx_paths_);
    if (action == "add")
      paths_.at(path)->DelElem(path, id);
    else
      paths_.at(path)->RestoreBackupObj();
    return ErrorCodes::GAME_NOT_RUNNING;
  }
}
