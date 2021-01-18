#include "page.h"
#include "nlohmann/json.hpp"
#include "util/error_codes.h"
#include "util/func.h"
#include <cctype>
#include <string>

namespace fs = std::filesystem;

Page::Page(std::string base_path, std::string path) {
  base_path_ = base_path;
  fs::path p = path;
  p.replace_extension("");
  std::string new_path = p;
  path_ = new_path;
  name_ = path_.substr(path_.rfind("/")+1);
  GenerateParentNodes();
}
    
nlohmann::json Page::CreatePageData(std::string path) {
  nlohmann::json json;
  json["path"] = "web/category_template.html";
  json["header"] = nlohmann::json({{"name", name_}, 
      {"nodes", nlohmann::json::array()}, {"__parents", parent_nodes_}});
  for (auto node : child_nodes_)
    json["header"]["nodes"].push_back(nlohmann::json({node.first, node.second}));
  return json;
}

void Page::GenerateParentNodes() {
  std::vector<std::string> path_elems = func::Split(path_.substr(base_path_.length()), "/");
  for (size_t i=2; i<path_elems.size(); i++) {
    std::string path = "";
    for (size_t j=0; j<=i; j++) 
      path += path_elems[j] + "/";
    path.pop_back();
    parent_nodes_[path] = path_elems[i];
  }
}

nlohmann::json Page::GetObjectsFromTemplate() {
  nlohmann::json objects;
  std::string path_to_templates = base_path_ + "/../templates/" + category_ + ".json";
  if (!func::LoadJsonFromDisc(path_to_templates, objects))
    return nlohmann::json({{"error", ErrorCodes::FAILED}});
  return objects;
}

nlohmann::json Page::GetObjectFromTemplate(std::string name) {
  // Get all objects.
  nlohmann::json objects = GetObjectsFromTemplate();  
  // Get first (as probably only) object from list. Also: any object will
  // probably do the job.
  nlohmann::json object = *objects.begin();
  // If object has unset id and/ or name field set field to given name.
  if (object.count("id")) 
    object["id"] = name;
  if (object.count("name") > 0) {
    name[0] = std::toupper(name[0]);
    object["name"] = name;
  }
  return object;
}
