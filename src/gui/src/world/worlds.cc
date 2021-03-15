#include "worlds.h"
#include "nlohmann/json.hpp"
#include "util/error_codes.h"
#include "util/func.h"
#include "world/world.h"
#include <algorithm>
#include <exception>
#include <iostream>
#include <mutex>
#include <ostream>
#include <shared_mutex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

const std::vector<std::string> Worlds::categories_ = {"attacks", "config", 
    "defaultDialogs", "dialogs", "players", "rooms", "characters", 
    "defaultDescriptions", "details", "items", "quests", "texts", "images"};

Worlds::Worlds(std::string base_path, int start_port) {
  base_path_ = base_path;
  ports_ = start_port;
  // Iterate over all users. 
  for (auto up : fs::directory_iterator(base_path_)) {
    std::string cur_path = up.path();
    // Iterate over user-worlds and create world elements.
    for (auto wp : fs::directory_iterator(cur_path + "/files")) {
      worlds_[wp.path()] = new World(base_path_, wp.path(), ports_); 
      ports_ += 2; // increate port (+1: game-http-server, +1: game-websocket-server.
    }
  }
}

Worlds::~Worlds() {
  for (auto it : worlds_) 
    delete it.second;
}

ErrorCodes Worlds::CreateNewWorld(std::string path, std::string name) {
  std::cout << "Worlds::AddWorld(" << path << ")" << std::endl;
  if (GetWorldFromUrl(path) != nullptr) 
    return ErrorCodes::ALREADY_EXISTS;
  
  std::string full_path = base_path_ + path;
  try {
    //Create directory for world
    fs::create_directories(full_path);

    //Create all subcategories.
    for (const auto& category : categories_)
      fs::create_directory(full_path + "/" + category);

    //Copy default config, room and player-file.
    fs::copy("../../data/default_jsons/config.json", full_path + "/config/"); 
    fs::copy("../../data/default_jsons/test.json", full_path + "/rooms/"); 
    fs::copy("../../data/default_jsons/players.json", full_path + "/players/"); 
    fs::copy("../../data/default_jsons/background.jpg", full_path + "/images/"); 
    
    // Create new world.
    std::unique_lock ul(shared_mtx_worlds_);
    worlds_[full_path] = new World(base_path_, full_path, ports_);
    ports_ += 2; // increate port (+1: game-http-server, +1: game-websocket-server.
  }
  catch (std::exception& e) {
    std::cout << "Failed creating directories of new world: " << e.what() << std::endl;
    fs::remove_all(path);
    return ErrorCodes::FAILED;
  }
  return ErrorCodes::SUCCESS;
}

ErrorCodes Worlds::DeleteWorld(std::string path) {
  std::cout << "Worlds::DeleteWorld(" << path << ")" << std::endl;
  if (GetWorldFromUrl(path) == nullptr)
    return ErrorCodes::NO_WORLD;
  
  // Write remove file.
  try {
    std::string full_path = base_path_ + path;
    fs::remove_all(full_path); 
    std::unique_lock ul(shared_mtx_worlds_);
    if (worlds_.count(full_path) > 0 && worlds_.at(full_path) != nullptr)
      delete worlds_.at(full_path);
    worlds_.erase(base_path_ + path);
  } catch (std::exception& e) {
    std::cout << "DeleteWorld failed: " << e.what() << std::endl;
    return ErrorCodes::FAILED;
  }
  return ErrorCodes::SUCCESS;
}

ErrorCodes Worlds::UpdateElements(std::string path, std::string name, std::string action, bool force,
    nlohmann::json obj) {
  std::cout << "Worlds::UpdateElements(" << path << ")" << std::endl;

  // Get matching world and return error when no matching world was found.
  World* world = GetWorldFromUrl(path);
  if (world == nullptr) 
    return ErrorCodes::NO_WORLD;

  // Call matching world-function: add, delete or modify element.
  if (action == "modify")
    return world->ModifyObject(base_path_ + path, name, obj, force);
  else if (action == "add")
    return world->AddElem(base_path_ + path, func::ConvertToId(name), obj, force);
  else if (action == "delete")
    return world->DelElem(base_path_ + path, func::ConvertToId(name), force);
  else
   return ErrorCodes::NOT_ALLOWED;
}

std::string Worlds::GetPage(std::string path) {
  std::cout << "Worlds::GetPage(" << path << ")" << std::endl;
  World* world = GetWorldFromUrl(path);
  if (world == nullptr) 
    return "No world found.";
  nlohmann::json json = world->GetPage(base_path_ + path);
  if (json.count("error") > 0) 
    return json["error"];
  return ParseTemplate(json);
}

std::string Worlds::ParseTemplate(nlohmann::json json) {
  std::cout << "Worlds::ParseTemplate()" << std::endl;
  inja::Environment env;
  inja::Template temp;
  env.add_void_callback("log", 1, [](inja::Arguments args) {
	  std::cout << "logging: " << args.at(0)->get<std::string>() << std::endl;
  });

  //Parse standard templates for descriptions and header and footer included in every object.
  inja::Template desc= env.parse_template("web/object_templates/description.html");
  env.include_template("web/object_templates/temp_description", desc);
  inja::Template text = env.parse_template("web/object_templates/text.html");
  env.include_template("web/object_templates/temp_text", text);
  inja::Template room_desc = env.parse_template("web/object_templates/room_description.html");
  env.include_template("web/object_templates/temp_room_description", room_desc);
  inja::Template dead_desc = env.parse_template("web/object_templates/dead_description.html");
  env.include_template("web/object_templates/temp_dead_description", dead_desc);
  inja::Template use_desc = env.parse_template("web/object_templates/use_description.html");
  env.include_template("web/object_templates/temp_use_description", use_desc);
  inja::Template pages = env.parse_template("web/object_templates/pages.html");
  env.include_template("web/object_templates/temp_pages", pages);
  inja::Template header = env.parse_template("web/object_templates/header.html");
  env.include_template("web/object_templates/temp_header", header);
  inja::Template footer = env.parse_template("web/object_templates/footer.html");
  env.include_template("web/object_templates/temp_footer", footer);

  // Get path and object, then crete template and render page.
  std::string page = "";
  try {
    std::string path = json["path"];
    temp = env.parse_template(path);
    page = env.render(temp, json["header"]);
  } catch (std::exception& e) {
    std::cout << "Problem parsing object html: " << e.what() << "\n\n";
    return "File not found.";
  }
  std::cout << std::endl;
  return page;
}

World* Worlds::GetWorldFromUrl(std::string path) {
  // Builds full path, as only
  std::string full_path = base_path_ + path;
  
  // worlds are only saved as "[base_path]/[user]/files/[world]" thus use of "find".
  std::unique_lock ul(shared_mtx_worlds_);
  auto it = std::find_if(worlds_.begin(), worlds_.end(), [&](auto it) 
      { return full_path.find(it.first) != std::string::npos; } );
  if (it != worlds_.end())
    return it->second;
  return nullptr;
}
