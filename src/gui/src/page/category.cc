#include "category.h" 
#include "nlohmann/json.hpp"

namespace fs = std::filesystem;

Category::Category(std::string base_path, std::string path) 
  : Page(base_path, path) {
  UpdateNodes();
}

nlohmann::json Category::RenderPage(std::string path) {
  std::cout << "Category::RenderPage()" << std::endl;
  return Page::RenderPage(path);
}

void Category::UpdateNodes() {
  for (auto& p : fs::directory_iterator(path_)) {
    std::string path = p.path();
    nodes_[path.substr(base_path_.length())] = p.path().stem();
  }
}
