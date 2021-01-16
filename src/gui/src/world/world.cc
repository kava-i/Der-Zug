#include "world.h"
#include "util/error_codes.h"
#include <iostream>
#include <ostream>
#include <string>

namespace fs = std::filesystem;

World::World(std::string base_path, std::string path, int port) {
  base_path_ = base_path;
  path_ = path;
  port_ = port;
  std::string temp = path.substr(base_path.length()+1);
  name_ = temp.substr(temp.rfind("/")+1);
  creator_ = temp.substr(0, temp.find("/"));
  InitializePaths(path_);
  UpdateShortPaths();
}

ErrorCodes World::AddElem(std::string path, std::string name, bool force) {
  std::cout << "World::AddElem(" << path << ")" << std::endl;
  if (paths_.count(path) > 0) {
    ErrorCodes error_code = paths_.at(path)->AddElem(path, name);
    if (error_code != ErrorCodes::SUCCESS) 
      return error_code;
    if (IsGameRunning() || force) {
      InitializePaths(path_);
      UpdateShortPaths();
      return ErrorCodes::SUCCESS;
    }
    else {
      paths_.at(path)->DelElem(path, name);
      return ErrorCodes::GAME_NOT_RUNNING;
    }
  }
  return ErrorCodes::PATH_NOT_FOUND;
}

ErrorCodes World::DelElem(std::string path, std::string name, bool force) {
  std::cout << "World::DelElem(" << path << ")" << std::endl;
  if (paths_.count(path) > 0) {
    ErrorCodes error_code = paths_.at(path)->DelElem(path, name);
    if (error_code != ErrorCodes::SUCCESS) 
      return error_code;
    if (IsGameRunning() || force) {
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

nlohmann::json World::GetPage(std::string path) {
  std::cout << "World::GetPage(" << path << ")" << std::endl;
  if (paths_.count(path) > 0) {
    nlohmann::json json = paths_.at(path)->CreatePageData(path);
    json["header"]["__short_paths"] = short_paths_;
    return json;
  }
  return nlohmann::json({{"error", "File not found."}});
}

// paths_
void World::InitializePaths(std::string path) {
  paths_[path] = new Category(base_path_, path);
  for (auto& p : fs::directory_iterator(path)) {
    std::cout << p.path() << ": ";
    std::string next_path = p.path();
    if (fs::is_directory(p) == true)
      InitializePaths(next_path);
    else {
      nlohmann::json objects;
      fs::path fs_p = next_path;
      fs_p.replace_extension("");
      std::string new_path = fs_p;
      if (p.path().extension() == ".jpg") 
        std::cout << "skipped image" << std::endl;
      else if (!func::LoadJsonFromDisc(next_path, objects))
        std::cout << "failed to load json." << std::endl;
      else if (objects.is_array() == false) {
        std::cout << "added area" << std::endl;
        paths_[new_path] = new Area(base_path_, new_path, objects);
        for (auto it=objects.begin(); it!=objects.end(); it++)
          paths_[new_path + "/" + it.key()] = paths_[new_path];
      }
      else
        std::cout << "skiped list type." << std::endl;
    }
  }
  std::cout << std::endl;
}

void World::UpdateShortPaths() {
  for (auto it : paths_)
    short_paths_.push_back(it.first.substr(base_path_.length()));
}

World::~World() {
  for (auto it : paths_) 
    delete it.second;
}

bool World::IsGameRunning() {
  //Create command
  std::string command = "./../../textadventure/build/bin/testing.o "
    "--path ../../data/users/" + creator_+"/files/" + name_ + "/";
  
  //Get players from players.json
  nlohmann::json players;
  if (func::LoadJsonFromDisc(path_ + "/players/players.json", players) == false) 
    return false;
  //Run game with every existing player.
  bool success = true;
  for (auto it=players.begin(); it!=players.end(); it++) {
    std::cout << "PLAYER: " << it.value() << std::endl;
    std::string test_p = command + " -p " + it.key() +
      " > ../../data/users/" + creator_ + "/logs/" + name_ + "_write.txt";
    if (system(test_p.c_str()) != 0) 
      success = false;
    std::cout << "Running with player \"" << it.key() << "\": " << success << std::endl;
  }
  std::cout << std::endl;
  return success;
}
