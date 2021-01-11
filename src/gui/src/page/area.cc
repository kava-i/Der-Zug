#include "area.h"

Area::Area(std::string base_path, std::string path, nlohmann::json objects) 
  : Page(base_path, path) {
  objects_ = objects;
  UpdateNodes();
}

nlohmann::json Area::RenderPage(std::string path) {
  std::cout << "Area::RenderPage(" << path << ")" << std::endl;
  nlohmann::json json;
  if (path != path_) {
    json = RenderObjectPage(path);
  }
  else 
    json = Page::RenderPage(path);
  return json;
}

nlohmann::json Area::RenderObjectPage(std::string path) {
  std::cout << "Area::RenderObjectPage(" << path << ")" << std::endl;
  nlohmann::json object;
  try { 
    object = objects_[path.substr(path.rfind("/"))];
  } catch (std::exception& e) {
    std::cout << "Area::RenderObjectPage(): object not loaded: " << e.what() << std::endl;
    return nlohmann::json({{"error", "Problem parsing json"}});
  }

  nlohmann::json json = nlohmann::json({{"header", nlohmann::json::object()}});
  json["header"]["name"] = object["id"];
  json["header"]["parents"] = parents_;
  json["object"] = object;
  json["path"] = "web/object_templates/" + parents_.end()->second + ".html";
  return json;
}

void Area::UpdateNodes() {
  for (auto it=objects_.begin(); it!=objects_.end(); it++)
    nodes_[path_ + "/" + it.key()] = it.key();
}
