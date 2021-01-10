#include "area.h"

Area::Area(std::string base_path, std::string path, nlohmann::json objects) 
  : Page(base_path, path) {
  objects_ = objects;
  UpdateNodes();
}

std::string Area::RenderPage(std::string path) {
  std::cout << "Category::RenderPage()" << std::endl;
  nlohmann::json json;
  if (path != path_) {
    json = RenderObjectPage(path);
    json["path"] = "web/object_templates/" + parents_.end().second() + ".html";
  }
  json = Page::RenderPage(path);
  return json;
}

void Area::UpdateNodes() {
  for (auto it=objects.begin(); it!=objects.end(); it++)
    nodes_[path_ + "/" + it.key()] = it.key();
}
