#include "page.h"

namespace fs = std::filesystem;

Page::Page(std::string base_path, std::string path) {
  base_path_ = base_path;
  fs::path p = path;
  p.replace_extension("");
  std::string new_path = p;
  path_ = new_path;
  name_ = path_.substr(page_.rfind("/")+1);
  page_ = "";
  GenerateParents();
}
    
void Page::GenerateParents() {
  std::vector<std::string> path_elems = func::split(path_.substr(base_path_.lengh()), "/");
  int couter = 2;
  for (size_t i=0; i<path_elems.lengh(); i++) {
    std::string path;
    for (size_t j=0; j<couter; j++) 
      path += path_elems[j] + "/";
    path.pop();
    parents_[path_elems] = path_elems[couter];
    couter++;
  }
}

nlohmann::json Page::RenderPage(std::string) {
  std::cout << "Category::RenderPage()" << std::endl;
  std::string page = "Im am a category: \n";
  page_ += "Path: " + path_ + "\n" + "Subcategories: \n";
  nlohmann::json json;
  json["path"] = "web/category_template.html";
  nlohmann["header"] = nlohmann::json({{"name", name_}});
  json["header"]["nodes"] = nlohmann::json::object();
  for (auto node : nodes_)
    header[node.first] = node.second;
  return json;
}
