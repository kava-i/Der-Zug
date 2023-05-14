#include "category.h" 
#include "nlohmann/json.hpp"
#include <filesystem>

namespace fs = std::filesystem;

Category::Category(std::string base_path, std::string path) 
  : Page(base_path, path) {
  GenerateChildNodes();
}

nlohmann::json Category::CreatePageData(std::string path) {
  std::cout << "Category::RenderPage()" << std::endl;
  return Page::CreatePageData(path);
}

void Category::GenerateChildNodes() {
  std::cout << "GenerateChildNodes" << std::endl;
  child_nodes_.clear();
  for (auto& p : fs::directory_iterator(path_)) {
    fs::path fs_path = p.path();
    // skip hidden files (f.e. the created data file)
    if (fs_path.filename().string().front() == '.') {
      std::cout << "  - omitting: " << fs_path.string() << std::endl;
      continue;
    }
    fs_path.replace_extension("");
    std::string path = fs_path;
    child_nodes_[path.substr(base_path_.length())] = p.path().stem();
  }
}
