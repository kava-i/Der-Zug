#include "worlds.h"
#include "nlohmann/json.hpp"
#include <exception>
#include <ostream>
#include <string>

namespace fs = std::filesystem;

Worlds::Worlds(std::string base_path, int start_port) {
  base_path_ = base_path;
  ports_ = start_port;

  // Iterate ver all users and user-worlds and create world elements.
  for (auto up : fs::directory_iterator(base_path_)) {
    std::string cur_path = up.path();
    std::cout << cur_path << std::endl;
    for (auto wp : fs::directory_iterator(cur_path + "/files")) {
      std::cout << wp.path() << std::endl;
      worlds_[wp.path()] = new World(base_path_, wp.path(), ports_); 
      ports_+=2;
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

std::string Worlds::GetPage(std::string path) {
  std::cout << "Worlds::GetPage(" << path << ")" << std::endl;
  // Builds full path, as only
  std::string full_path = base_path_ + path;
  // words are only saved as "[base_path]/[user]/files/[world]" thus use of "find".
  for (auto it : worlds_) {
    if (full_path.find(it.first) != std::string::npos) {
      nlohmann::json json = it.second->GetPage(full_path);
      return ParseTemplate(json);
    }
  }
  return "No world found.";
}

std::string Worlds::ParseTemplate(nlohmann::json json) {
  std::cout << "Worlds::ParseTemplate(" << json << ")" << std::endl;
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
    std::cout << "Path: " << path << std::endl;
      
    std::cout << 5 << std::endl;
    try { 
      temp = env.parse_template(path);
    } catch(std::exception& e){
      std::cout << "Problem parsing object html: " << e.what() << std::endl;
    }

    std::cout << 6 << std::endl;
    page = env.render(temp, json["header"]);
    std::cout << 7 << std::endl;
  }
  catch (std::exception& e) {
    std::cout << "Problem parsing object html: " << e.what() << std::endl;
    return "File not found.";
  }
  return page;
}
