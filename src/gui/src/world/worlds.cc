#include "worlds.h"
#include "nlohmann/json.hpp"
#include "util/error_codes.h"
#include "world/world.h"
#include <exception>
#include <iostream>
#include <ostream>
#include <string>

namespace fs = std::filesystem;

Worlds::Worlds(std::string base_path, int start_port) {
  base_path_ = base_path;
  ports_ = start_port;
  // Iterate ver all users and user-worlds and create world elements.
  for (auto up : fs::directory_iterator(base_path_)) {
    std::string cur_path = up.path();
    for (auto wp : fs::directory_iterator(cur_path + "/files")) {
      worlds_[wp.path()] = new World(base_path_, wp.path(), ports_); 
      ports_+=2;
    }
  }
}

Worlds::~Worlds() {
  for (auto it : worlds_) delete it.second;
}

ErrorCodes Worlds::AddElem(std::string path, std::string name, bool force) {
  std::cout << "Worlds::AddElem(" << path << ")" << std::endl;
  World* world = GetWorld(path);
  if (world == nullptr) 
    return ErrorCodes::NO_WORLD;
  return world->AddElem(base_path_ + path, name, force);
}

ErrorCodes Worlds::DelElem(std::string path, std::string name, bool force) {
  std::cout << "Worlds::DelElem(" << path << ")" << std::endl;
  World* world = GetWorld(path);
  if (world == nullptr) 
    return ErrorCodes::NO_WORLD;
  return world->DelElem(base_path_ + path, name, force);
}

std::string Worlds::GetPage(std::string path) {
  std::cout << "Worlds::GetPage(" << path << ")" << std::endl;
  World* world = GetWorld(path);
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
  }
  catch (std::exception& e) {
    std::cout << "Problem parsing object html: " << e.what() << "\n\n";
    return "File not found.";
  }
  std::cout << std::endl;
  return page;
}

World* Worlds::GetWorld(std::string path) {
  // Builds full path, as only
  std::string full_path = base_path_ + path;
  // words are only saved as "[base_path]/[user]/files/[world]" thus use of "find".
  for (auto it : worlds_) {
    if (full_path.find(it.first) != std::string::npos) 
      return it.second;
  }
  return nullptr;
}
