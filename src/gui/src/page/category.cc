#include "category.h" 
#include "nlohmann/json.hpp"
#include <filesystem>

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
    fs::path fs_path = p.path();
    fs_path.replace_extension("");
    std::string path = fs_path;
    nodes_[path.substr(base_path_.length())] = p.path().stem();
  }
}
