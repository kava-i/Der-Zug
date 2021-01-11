#include "category.h" 
#include "nlohmann/json.hpp"
#include <string>

namespace fs = std::filesystem;

Category::Category(std::string base_path, std::string path) 
  : Page(base_path, path) {
  UpdateNodes();
}

nlohmann::json Category::RenderPage(std::string) {
  std::cout << "Category::RenderPage()" << std::endl;
  nlohmann::json json;
  json["path"] = "web/category_template.html";
  json["header"] = nlohmann::json({{"name", name_}, {"nodes", nlohmann::json::array()}});
  for (auto node : nodes_) 
    json["header"]["nodes"].push_back(nlohmann::json({node.first, node.second}));
  return json;
}

void Category::UpdateNodes() {
  for (auto& p : fs::directory_iterator(path_)) {
    std::string path = p.path();
    nodes_[path.substr(base_path_.length())] = p.path().stem();
  }
}
