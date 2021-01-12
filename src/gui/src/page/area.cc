#include "area.h"
#include "nlohmann/json.hpp"

Area::Area(std::string base_path, std::string path, nlohmann::json objects) 
  : Page(base_path, path) {
  objects_ = objects;
  UpdateNodes();
}

nlohmann::json Area::RenderPage(std::string path) {
  std::cout << "Area::RenderPage(" << path << ")" << std::endl;
  nlohmann::json json;
  if (path != path_)
    json = RenderObjectPage(path);
  else
    json = Page::RenderPage(path);
  return json;
}

nlohmann::json Area::RenderObjectPage(std::string path) {
  std::cout << "Area::RenderObjectPage(" << path << ")" << std::endl;
  nlohmann::json object;
  if (objects_.count(path.substr(path.rfind("/")+1)) == 0)
    return nlohmann::json({{"error", "Problem parsing json"}});
  object = objects_[path.substr(path.rfind("/")+1)];
  object["__parents"] = parents_;
  nlohmann::json json = nlohmann::json({{"header", object}, 
      {"path", "web/object_templates/" + parents_.rbegin()->second + ".html"}});
  std::cout << "Area::RenderObjectPage(): sucess." << std::endl;
  return json;
}

void Area::UpdateNodes() {
  for (auto it=objects_.begin(); it!=objects_.end(); it++)
    nodes_[path_.substr(base_path_.length()) + "/" + it.key()] = it.key();
}
