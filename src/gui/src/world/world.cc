#include "world.h"
#include "nlohmann/json.hpp"
#include "page/media.h"
#include "page/page.h"
#include "page/sub_category.h"
#include "util/error_codes.h"
#include "util/func.h"
#include <algorithm>
#include <exception>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <ostream>
#include <set>
#include <shared_mutex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

#define NOTES_PATH "/.data/notes.json"

World::World(std::string base_path, std::string path, std::string id, std::string name) 
  : base_path_(base_path), path_(path), id_(id), name_(name) {
  // Parse name and creator from path.
  std::string temp = path.substr(base_path.length()+1);
  creator_ = temp.substr(0, temp.find("/"));

  // Load notes.
  nlohmann::json notes;
  if (func::LoadJsonFromDisc(path_ + NOTES_PATH, notes)) {
    notes_ = notes.get<std::map<std::string, std::string>>();
  }

  // Init paths (map of full-path + page-object) and short_paths (vector of url-paths).
  LoadWorld();
  UpdateShortPaths();
}

std::string World::id() const {
  return id_;
}

std::string World::name() const {
  return name_;
}

std::string World::creator() const {
  return creator_;
}

ErrorCodes World::ModifyObject(std::string path, std::string id, nlohmann::json modified_obj, bool force) {
  std::cout << "World::ModifyObject" << std::endl;
   // Check that path exists.
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) == 0) 
    return ErrorCodes::PATH_NOT_FOUND;
  
  // Modify object. If not successfull or force-write (thus no need to do further checking), return error-code.
  ErrorCodes error_code = paths_.at(path)->ModifyObject(path, id, modified_obj);
  sl.unlock();
  // If successfull and force disabled, make revert, or make changes permanent.
  if (error_code == ErrorCodes::SUCCESS && !force)
    error_code = RevertIfGameNotRunning(path, id, "modify");

  // Refresh world and return error-code.
  RefreshWorld(); 
  return error_code;
}

ErrorCodes World::AddElem(std::string path, std::string id, nlohmann::json infos, bool force) {
  std::cout << "World::AddElem(" << path << ", " << force << ")" << std::endl;
  // Check that path exists.
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) == 0) 
    return ErrorCodes::PATH_NOT_FOUND;

  // Add element. If not successfull or force-write (thus no need to do further checking), return error-code.
  ErrorCodes error_code = paths_.at(path)->AddElem(path, id, infos);
  sl.unlock();
  // If successfull and force disabled, make revert, or make changes permanent.
  if (error_code == ErrorCodes::SUCCESS && !force)
    error_code = RevertIfGameNotRunning(path, id, "add");

  // Refresh world and return error-code.
  RefreshWorld(); 
  return error_code;
}

ErrorCodes World::DelElem(std::string path, std::string id, bool force) {
  std::cout << "World::DelElem(" << path << ")" << std::endl;
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) == 0) {
    return ErrorCodes::PATH_NOT_FOUND;
  }

  // Delete element. If not successfull or force-write (thus no need to do further checking), return error-code.
  ErrorCodes error_code = paths_.at(path)->DelElem(path, id);
  sl.unlock();
  std::cout << "Checking error-code: " << error_code << ". Force: " << force << std::endl;
  // If successfull and force disabled, make revert, or make changes permanent.
  if (error_code == ErrorCodes::SUCCESS && !force)
    error_code = RevertIfGameNotRunning(path, id, "delete");
  
  // If path not found, try again with calling element directly (skip
  // sub-category).
  if (error_code == ErrorCodes::PATH_NOT_FOUND) {
    if (paths_.contains(path + "/" + id))
      error_code = paths_.at(path + "/" + id)->DelElem(path, id);
  }

  // Refresh world and return error-code.
  RefreshWorld(); 
  return error_code;
}


nlohmann::json World::GetPage(std::string path, bool only_json) const {
  std::cout << "World::GetPage(" << path << ")" << std::endl;
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) == 0)
    return nlohmann::json({{"error", "File not found."}});
  nlohmann::json json = paths_.at(path)->CreatePageData(path);
  // If only json, we extract all actual lements from the object and return directly.
  if (only_json) {
    nlohmann::json object = json["header"];
    object.erase("__parents");
    return object;
  }

  nlohmann::json config;
  func::LoadJsonFromDisc(path_ + "/config/config.json", config);
  nlohmann::json attribute_config;
  func::LoadJsonFromDisc(path_ + "/config/attributes.json", attribute_config);
  nlohmann::json item_config;
  func::LoadJsonFromDisc(path_ + "/config/items.json", item_config);
  nlohmann::json short_paths = short_paths_;
  json["header"]["__short_paths"] = short_paths.dump();
  json["header"]["__is_main"] = (path == path_);
  json["header"]["__creator"] = creator_;
  json["header"]["__world_name"] = id_;
  json["header"]["__categories"] = item_config["categories"];
  json["header"]["__kinds"] = item_config["kinds"];
  json["header"]["__types"] = item_config["types"];
  json["header"]["__attribute_categories"] = GetAttributeCategories();
  json["header"]["__attributes"] = GetAttributes();
  json["header"]["__full_attributes"] = attribute_config["attributes"];
	
	// Data from which to construct select-fields with javascript
  nlohmann::json attributes = GetAttributes();
  json["header"]["_attributes"] = GetAttributes();
  json["header"]["_mod_types"] = GetModTypes();
  json["header"]["_colors"] = {"", "green", "red", "white"};

  nlohmann::json availibe_fields;
  func::LoadJsonFromDisc("handlers.json", availibe_fields);
  json["header"]["__availibe_handlers"] = availibe_fields;
  func::LoadJsonFromDisc("commands.json", availibe_fields);
  json["header"]["__availibe_commands"] = availibe_fields;
  func::LoadJsonFromDisc("handler_arguments.json", availibe_fields);
  json["header"]["__handler_arguments"] = availibe_fields.dump();
  func::LoadJsonFromDisc("event_arguments.json", availibe_fields);
  json["header"]["__event_arguments"] = availibe_fields.dump();

  return json;
}

nlohmann::json World::GetGraph(std::string path) const {
  std::cout << "World::GetGraph(" << path << ")" << std::endl;
  std::shared_lock sl(shared_mtx_paths_);
  if (paths_.count(path) == 0)
    return nlohmann::json::object();
  return paths_.at(path)->GetGraph(path);
}

nlohmann::json World::GetNotes(std::string path) const {
  std::cout << "World::GetGraph(" << path << ")" << std::endl;
  std::shared_lock sl(shared_mtx_notes_);
  if (notes_.count(path) == 0)
    return nlohmann::json::object();
  return notes_.at(path);
}

ErrorCodes World::SetNotes(std::string path, std::string notes) {
  std::cout << "World::SetNotes(" << path << ")" << std::endl;
  std::shared_lock sl(shared_mtx_paths_);
  // If path does not exist, don't add note.
  if (paths_.count(path) == 0)
    return ErrorCodes::PATH_NOT_FOUND;
  sl.unlock();

  // Overwrite or set new note and safe all notes to disc.
  std::unique_lock ul(shared_mtx_notes_);
  notes_[path] = notes;
  nlohmann::json notes_json = notes_;
  func::WriteJsonToDisc(path_ + NOTES_PATH, notes_json);
  return ErrorCodes::SUCCESS;
}

// paths_
void World::LoadWorld() {
  std::unique_lock ul(shared_mtx_paths_);
  paths_[path_] = new Category(base_path_, path_);
  ul.unlock();
  // category => elements are directories (we no, what all categories and can check).
  for (auto& p : fs::directory_iterator(path_)) {
    if (p.path() != path_ + "/data")
      InitializeCategory(p.path());
  }
}
void World::InitializeCategory(std::string path) {
  std::cout << "InitializePaths: " << path << std::endl;
  // subcategories => elements are files (jsons).
  paths_[path] = new SubCategory(base_path_, path);
  std::cout << "Added Subcategory: " << path << std::endl;
  for (auto& p : fs::directory_iterator(path)) {
    nlohmann::json objects;
    std::string path_without_extension = func::RemoveExtension(p.path());
    if (p.path().extension() == ".jpg" || p.path().extension() == ".mp3") {
      std::string type = (p.path().extension() == ".jpg") ? "image" : "sound";
      paths_[path_without_extension] = new Media(base_path_, path_without_extension, 
          type, p.path().extension());
    }
    else if (!func::LoadJsonFromDisc(p.path(), objects))
      std::cout << "failed to load json." << std::endl;
    else if (objects.is_array() == false) {
      paths_[path_without_extension] = new Area(base_path_, path_without_extension, objects);
      // objects: objects in json of objects.
      for (auto it=objects.begin(); it!=objects.end(); it++)
        paths_[path_without_extension + "/" + it.key()] = paths_.at(path_without_extension);
    }
    else
      std::cout << "skiped list type." << std::endl;
  }
  std::cout << "Done initializing paths under: " << path << "." << std::endl;
}

void World::UpdateShortPaths() {
  std::shared_lock sl(shared_mtx_paths_);
  short_paths_.clear();
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
    "--path ../../data/users/" + creator_ + "/files/" + id_ + "/";
  
  //Get players from players.json
  nlohmann::json players;
  if (func::LoadJsonFromDisc(path_ + "/players/players.json", players) == false) {
    std::cout << "Failed to load player json: " << path_ << "/players/players.json" << std::endl;
    return false;
  }

  //Run game with every existing player.
  bool success = true;
  for (auto it=players.begin(); it!=players.end(); it++) {
    std::string test_p = command + " -p " + it.key() +
      " > ../../data/users/" + creator_ + "/logs/" + id_ + "_write.txt";
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
  std::cout << "Checking game running .. " << path << ", " << id << ", " << action << std::endl;
  ErrorCodes error_code = ErrorCodes::SUCCESS;
  // If game is not running, revert changes.
  if (!IsGameRunning()) {
    std::shared_lock sl(shared_mtx_paths_);
    if (action == "add") {
      std::cout << "Game not running, deleting element." << std::endl;
      paths_.at(path)->DelElem(path, id);
      std::cout << "Element deteled." << std::endl;
    }
    else {
      std::cout << "Game not running, restoring element." << std::endl;
      paths_.at(path)->RestoreBackupObj(id);
      std::cout << "Element restored." << std::endl;
    }
    error_code = ErrorCodes::GAME_NOT_RUNNING;
  }
  return error_code;
}

void World::RefreshWorld() {
  // Recreate paths and short-paths.
  LoadWorld();
  UpdateShortPaths();
}

std::map<std::string, std::string> World::GetAttributes() const {
	nlohmann::json config;
  func::LoadJsonFromDisc(path_ + "/config/attributes.json", config);
	std::map<std::string, std::string> attributes;
	if (config.contains("attributes") && config["attributes"].size() > 0) {
		for (const auto& attribute: config["attributes"].get<std::vector<nlohmann::json>>()) {
			if (attribute.contains("id"))
				attributes[attribute["id"]] = attribute.value("name", attribute["id"]);
		}
	}
	std::cout << "Got " << attributes.size() << " attributes" << std::endl;
	return attributes;
}
std::vector<std::string> World::GetAttributeCategories() const {
	nlohmann::json config;
  func::LoadJsonFromDisc(path_ + "/config/attributes.json", config);
	std::set<std::string> categories;
	std::vector<std::string> vec_categories;
	if (config.contains("attributes") && config["attributes"].size() > 0) {
		for (const auto& attribute: config["attributes"].get<std::vector<nlohmann::json>>()) {
			if (attribute.contains("category"))
				categories.insert(attribute["category"]);
		}
		vec_categories.assign(categories.begin(), categories.end());
	}
	std::cout << "Got " << vec_categories.size() << " attribute-categories" << std::endl;
	return vec_categories;
}

std::vector<std::string> World::GetModTypes() const {
	return {"=", "+", "-", "*", "/"};
}
