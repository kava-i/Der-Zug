#include "page.h"
#include "nlohmann/json.hpp"
#include "util/error_codes.h"
#include "util/func.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <numeric>
#include <string>

namespace fs = std::filesystem;

Page::Page(std::string base_path, std::string path) {
  base_path_ = base_path;
  fs::path p = path;
  p.replace_extension("");
  std::string new_path = p;
  path_ = new_path;
  name_ = func::GetLastElemFromPath(path);
  notes_ = "";
  GenerateParentNodes();
  std::ifstream read(base_path_ + "/" + path_ + "/" + name_ + ".md");
  if (read) {
    read >> notes_;
    read.close();
  }
}

std::string Page::name() const { 
  return name_; 
}

std::string Page::notes(std::string) const {
  return notes_;
}

void Page::set_notes(std::string, std::string description) {
  notes_ = description;
  std::ofstream write(base_path_ + "/" + path_ + "/" + name_ + ".md");
  write << notes_;
  write.close();
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
    std::string path = std::accumulate(path_elems.begin()+1, path_elems.begin()+i+1, 
        *path_elems.begin(), [](const std::string& init, std::string& str) { return init + "/" + str; });
    parent_nodes_[path] = path_elems[i];
  }
}

bool Page::GetObjectsFromTemplate(nlohmann::json& template_area) {
  std::string category = (category_ == "") ? name_ : category_;
  std::string path_to_templates = base_path_ + "/../templates/" + category + ".json";
  return func::LoadJsonFromDisc(path_to_templates, template_area);
}

bool Page::GetObjectFromTemplate(nlohmann::json& object, std::string name) {
  // Get all objects.
  nlohmann::json objects;
  if (!GetObjectsFromTemplate(objects)) 
    return false;
  // Get first (as probably only) object from list.
  object = *objects.begin();
  // If object has id and/ or name field set field to given name.
  if (object.count("id") > 0)
    object["id"] = name;
  if (object.count("name") > 0)
    object["name"] = func::ConvertFromId(name);
  return true;
}
