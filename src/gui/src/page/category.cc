#include "category.h" 

Category::Category(std::string base_path, std::string path) 
  : Page(base_path, path) {
  UpdateNodes();
}

nlohmann::json Category::RenderPage(std::string path) {
  return Page::RenderPage(path);
}

void Category::UpdateNodes() {
  for (auto& p : fs::directory_iterator(path))
    nodes_[p.path()] = p.path().filename();
}
