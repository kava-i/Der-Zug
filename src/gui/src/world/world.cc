#include "world.h"

namespace fs = std::filesystem;

World::World(std::string base_path, std::string path, int port) {
  base_path_ = base_path;
  path_ = path;
  port_ = port;
  InitializePaths(path_);
  UpdateShortPaths();
}

nlohmann::json World::GetPage(std::string path) {
  std::cout << "World::GetPage(" << path << ")" << std::endl;
  if (paths_.count(path) > 0) {
    nlohmann::json json = paths_.at(path)->CreatePageData(path);
    json["header"]["__short_paths"] = short_paths_;
    return json;
  }
  return nlohmann::json({{"error", "File not found."}});
}

// paths_
void World::InitializePaths(std::string path) {
  paths_[path] = new Category(base_path_, path);
  for (auto& p : fs::directory_iterator(path)) {
    std::cout << p.path() << ": ";
    std::string next_path = p.path();
    if (fs::is_directory(p) == true)
      InitializePaths(next_path);
    else {
      nlohmann::json objects;
      fs::path fs_p = next_path;
      fs_p.replace_extension("");
      std::string new_path = fs_p;
      if (p.path().extension() == ".jpg") 
        std::cout << "skipped image" << std::endl;
      else if (!func::LoadJsonFromDisc(next_path, objects))
        std::cout << "failed to load json." << std::endl;
      else if (objects.is_array() == false) {
        std::cout << "added area" << std::endl;
        paths_[new_path] = new Area(base_path_, new_path, objects);
        for (auto it=objects.begin(); it!=objects.end(); it++)
          paths_[new_path + "/" + it.key()] = paths_[new_path];
      }
      else
        std::cout << "skiped list type." << std::endl;
    }
  }
}

void World::UpdateShortPaths() {
  for (auto it : paths_)
    short_paths_.push_back(it.first.substr(base_path_.length()));
}

World::~World() {
  for (auto it : paths_) 
    delete it.second;
}
