#include "page.h"

namespace fs = std::filesystem;

Page::Page(std::string base_path, std::string path) {
  base_path_ = base_path;
  fs::path p = path;
  p.replace_extension("");
  std::string new_path = p;
  path_ = new_path;
  name_ = path_.substr(path_.rfind("/")+1);
  GenerateParents();
}
    
nlohmann::json Page::RenderPage(std::string path) {
  nlohmann::json json;
  json["path"] = "web/category_template.html";
  json["header"] = nlohmann::json({{"name", name_}, 
      {"nodes", nlohmann::json::array()}, {"__parents", parents_}});
  for (auto node : nodes_)
    json["header"]["nodes"].push_back(nlohmann::json({node.first, node.second}));
  return json;
}

std::string addup(std::string str) { return str + "/"; }

void Page::GenerateParents() {
  std::vector<std::string> path_elems = func::Split(path_.substr(base_path_.length()), "/");
  for (size_t i=2; i<path_elems.size(); i++) {
    std::string path = "";
    for (size_t j=0; j<=i; j++) 
      path += path_elems[j] + "/";
    path.pop_back();
    parents_[path] = path_elems[i];
  }
}
