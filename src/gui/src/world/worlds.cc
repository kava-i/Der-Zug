#include "worlds.h"
#include <ostream>

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

  //Parse different objects.
  try {
    temp = env.parse_template(json["path"]);
  }
  catch (std::exception& e) {
    std::cout << "Problem parsing object html: " << e.what() << std::endl;
    return "File not found.";
  }
  return env.render(temp, json["header"]);
}
