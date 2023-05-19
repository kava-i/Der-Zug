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

const std::vector<std::string> Worlds::categories_ = {"attacks", "config", "dialogs", 
  "players", "rooms", "characters", "details", "items", "quests", "texts", "images"};

Worlds::Worlds(std::string base_path) {
  base_path_ = base_path;
  // Iterate over all users. 
  for (auto up : fs::directory_iterator(base_path_)) {
    std::string cur_path = up.path();
    // Iterate over user-worlds and create world elements.
    for (auto wp : fs::directory_iterator(cur_path + "/files")) {
      // Load infos in .data folder and create new world:
      nlohmann::json infos;
      func::LoadJsonFromDisc(wp.path().string() + "/.data/infos.json", infos);
      try {
        worlds_[wp.path()] = new World(base_path_, wp.path(), infos["id"], infos["name"]); 
      } catch (std::exception &e) {
        std::cout << "Failed loading world from info: " << infos.dump() << e.what() << std::endl;
      }
      // Create all subcategories.
      for (const auto& category : categories_)
        fs::create_directory(wp.path().string() + "/" + category);
    }
  }
  std::cout << "Done creating worlds." << std::endl;
}

Worlds::~Worlds() {
  for (auto it : worlds_) 
    delete it.second;
}

const std::map<std::string, World*>& Worlds::worlds() {
  return worlds_;
}

ErrorCodes Worlds::CreateNewWorld(std::string path, std::string world_id, nlohmann::json infos) {
  std::cout << "Worlds::AddWorld(" << path << ")" << std::endl;
  if (GetWorldFromUrl(path) != nullptr) 
    return ErrorCodes::ALREADY_EXISTS;
  
  std::string full_path = base_path_ + path;
  try {
    // get languge
    std::string lang = infos.value("language", "en");
    std::cout << "got language: " << lang << std::endl;
    if (lang != "de" && lang != "en")
      lang = "en";
    std::cout << "using language: " << lang << std::endl;

    // Create directory for world
    fs::create_directories(full_path);

    // Create all subcategories.
    for (const auto& category : categories_)
      fs::create_directory(full_path + "/" + category);
    fs::create_directory(full_path + "/.data"); // data is a folder necessary but no category.

    //Copy default config, room and player-file.
    fs::copy("../../data/default_jsons/" + lang + "_config.json", full_path + "/config/config.json"); 
    fs::copy("../../data/default_jsons/" + lang + "_config_attributes.json", 
        full_path + "/config/attributes.json"); 
    fs::copy("../../data/default_jsons/" + lang + "_config_items.json", 
        full_path + "/config/items.json"); 
    fs::copy("../../data/default_jsons/test.json", full_path + "/rooms/"); 
    fs::copy("../../data/default_jsons/players.json", full_path + "/players/"); 
    fs::copy("../../data/default_jsons/background.jpg", full_path + "/images/"); 

    // Create new world.
    std::unique_lock ul(shared_mtx_worlds_);
    worlds_[full_path] = new World(base_path_, full_path, world_id, infos["name"]);
    // Add id to infos and store infos to disc: 
    infos["id"] = world_id;
    func::WriteJsonToDisc(full_path + "/.data/infos.json", infos);
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

ErrorCodes Worlds::UpdateElements(std::string path, std::string world_id, 
    std::string action, bool force, nlohmann::json obj) {
  std::cout << "Worlds::UpdateElements(" << path << ")" << std::endl;

  // Get matching world and return error when no matching world was found.
  World* world = GetWorldFromUrl(path);
  if (world == nullptr) 
    return ErrorCodes::NO_WORLD;

  // Call matching world-function: add, delete or modify element.
  if (action == "modify")
    return world->ModifyObject(base_path_ + path, world_id, obj, force);
  else if (action == "add")
    return world->AddElem(base_path_ + path, func::ConvertToId(world_id), obj, force);
  else if (action == "delete")
    return world->DelElem(base_path_ + path, func::ConvertToId(world_id), force);
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

nlohmann::json Worlds::GetObjectJson(std::string path) {
  World* world = GetWorldFromUrl(path);
  if (world == nullptr) {
    return nlohmann::json();
  }
  return  world->GetPage(base_path_ + path, true);
}

nlohmann::json Worlds::GetGraph(std::string path) {
  World* world = GetWorldFromUrl(path);
  if (world == nullptr) {
    return nlohmann::json();
  }
  return  world->GetGraph(base_path_ + path);
}

nlohmann::json Worlds::GetNotes(std::string path) {
  World* world = GetWorldFromUrl(path);
  if (world == nullptr) {
    return nlohmann::json();
  }
  return  world->GetNotes(base_path_ + path);
}

void Worlds::SetNotes(std::string path, std::string notes) {
  World* world = GetWorldFromUrl(path);
  if (world != nullptr) {
    world->SetNotes(base_path_ + path, notes);
  }
}

std::string Worlds::ParseTemplate(nlohmann::json json) {
  std::cout << "Worlds::ParseTemplate()" << std::endl;
  inja::Environment env;
  inja::Template temp;
  env.add_void_callback("log", 1, [](inja::Arguments args) {
	  std::cout << "logging: " << args.at(0)->get<std::string>() << std::endl;
  });
	
  // Parse standard templates for elements of pages
	std::cout << "Loading element-templates" << std::endl;
  inja::Template listener = env.parse_template("web/element_templates/listeners.html");
  env.include_template("web/element_templates/listeners", listener);
  inja::Template events = env.parse_template("web/element_templates/events.html");
  env.include_template("web/element_templates/events", events);
	std::cout << "done loading element-templates" << std::endl;
  inja::Template updates = env.parse_template("web/element_templates/updates.html");
  env.include_template("web/element_templates/updates", updates);
	std::cout << "done loading update-templates" << std::endl;

  // Parse standard templates for objects
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
  // Parse other standard templates
  inja::Template header = env.parse_template("web/object_templates/header.html");
  env.include_template("web/object_templates/temp_header", header);
  inja::Template footer = env.parse_template("web/object_templates/footer.html");
  env.include_template("web/object_templates/temp_footer", footer);

  // Get path and object, then crete template and render page.
  std::string page = "";
  try {
    std::cout << "Before render" << json.dump() << std::endl;
    std::string path = json["path"];
    if (!func::demo_exists(json["path"]))
      path = json["path2"];
    std::cout << "Loading template: " << path << std::endl;
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
  std::cout << "GetWorldFromUrl: " << full_path << std::endl;
  
  // worlds are only saved as "[base_path]/[user]/files/[world]" thus use of "find".
  std::unique_lock ul(shared_mtx_worlds_);
  auto it = std::find_if(worlds_.begin(), worlds_.end(), [&](auto it) 
      { return full_path.find(it.first) != std::string::npos; } );
  if (it != worlds_.end())
    return it->second;
  return nullptr;
}

World* Worlds::GetWorld(std::string creator, std::string world_id) {
  for (const auto& it : worlds_) {
    if (it.second->creator() == creator && it.second->id() == world_id)
      return it.second;
  }
  return nullptr;
}
