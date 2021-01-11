#include "category.h" 

namespace fs = std::filesystem;

Category::Category(std::string base_path, std::string path) 
  : Page(base_path, path) {
  UpdateNodes();
}

nlohmann::json Category::RenderPage(std::string path) {
  return Page::RenderPage(path);
}

void Category::UpdateNodes() {
  std::cout << "Category::RenderPage(" << path << ")" << std::endl;
  for (auto& p : fs::directory_iterator(path_))
    nodes_[p.path()] = p.path().filename();
}
