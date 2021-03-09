#include "world.h"
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
  std::cout << "Creating world: " << base_path << ", " << path << std::endl;
  std::string temp = path.substr(base_path.length()+1);
  name_ = temp.substr(temp.rfind("/")+1);
  creator_ = temp.substr(0, temp.find("/"));
  InitializePaths(path_);
  UpdateShortPaths();
  std::cout << "Done creating world.\n\n" << std::endl;
}

int World::port() const {
  return port_;
}

ErrorCodes World::AddElem(std::string path, std::string id, bool force) {
  std::cout << "World::AddElem(" << path << ")" << std::endl;
  // Convert id to lower-case and replace spaces with underscores.
  ConvertId(id);

  // Check that path exists.
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) > 0) {
    ErrorCodes error_code;
    // Add element.
    try {
      error_code = paths_.at(path)->AddElem(path, id);
    } catch (std::exception& e) {
      std::cout << "Failed due to error: " << e.what() << std::endl;
      return ErrorCodes::FAILED;
    }
    if (error_code != ErrorCodes::SUCCESS)
      return error_code;

    // Only make changes permanent, if game is still running (or force-write == true)
    if (IsGameRunning() || force) {
      sl.unlock();
      InitializePaths(path_);
      UpdateShortPaths();
      return ErrorCodes::SUCCESS;
    }
    // Delete newly added element otherwise.
    else {
      paths_.at(path)->DelElem(path, id);
      return ErrorCodes::GAME_NOT_RUNNING;
    }
  }
  return ErrorCodes::PATH_NOT_FOUND;
}

ErrorCodes World::DelElem(std::string path, std::string name, bool force) {
  std::cout << "World::DelElem(" << path << ")" << std::endl;
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) > 0) {
    ErrorCodes error_code = paths_.at(path)->DelElem(path, name);
    if (error_code != ErrorCodes::SUCCESS) 
      return error_code;
    if (IsGameRunning() || force) {
      sl.unlock();
      InitializePaths(path_);
      UpdateShortPaths();
      return ErrorCodes::SUCCESS;
    }
    else {
      std::cout << "Game not running, undoing changes!" << std::endl;
      paths_.at(path)->UndoDelElem();
      return ErrorCodes::GAME_NOT_RUNNING;
    }
  }
  return ErrorCodes::PATH_NOT_FOUND;
}

nlohmann::json World::GetPage(std::string path) const {
  std::cout << "World::GetPage(" << path << ")" << std::endl;
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) > 0) {
    nlohmann::json json = paths_.at(path)->CreatePageData(path);
    json["header"]["__short_paths"] = short_paths_;
    return json;
  }
  return nlohmann::json({{"error", "File not found."}});
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
  std::cout << "Update paths: " << std::endl;
  std::shared_lock sl(shared_mtx_paths_);
  for (auto it : paths_)
    short_paths_.push_back(it.first.substr(base_path_.length()));
  std::cout << "Done." << std::endl;
}

World::~World() {
  std::unique_lock ul(shared_mtx_paths_);
  while (paths_.size() > 0) {
    auto it = paths_.begin();
    // std::string name = it->second->name();
    std::erase_if(paths_, [&](const auto& elem) { 
        return (elem.second->category() == it->second->name()); 
      });
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
