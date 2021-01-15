#include "area.h"
#include "nlohmann/json.hpp"

Area::Area(std::string base_path, std::string path, nlohmann::json objects) 
  : Page(base_path, path) {
  objects_ = objects;
  UpdateNodes();
}

nlohmann::json Area::CreatePageData(std::string path) {
  std::cout << "Area::RenderPage(" << path << ")" << std::endl;
  if (path != path_) return CreateObjectPageData(path);
  return Page::CreatePageData(path);
}

nlohmann::json Area::CreateObjectPageData(std::string path) {
  std::cout << "Area::RenderObjectPage(" << path << ")" << std::endl;
  if (objects_.count(path.substr(path.rfind("/")+1)) == 0)
    return nlohmann::json({{"error", "Problem parsing json"}});
  nlohmann::json object = objects_[path.substr(path.rfind("/")+1)];
  object["__parents"] = parent_nodes_;
  nlohmann::json json = nlohmann::json({{"header", object}, 
      {"path", "web/object_templates/" + (--parent_nodes_.rbegin())->second + ".html"}});
  return json;
}

void Area::UpdateNodes() {
  for (auto it=objects_.begin(); it!=objects_.end(); it++)
    child_nodes_[path_.substr(base_path_.length()) + "/" + it.key()] = it.key();
}
