#include "page.h"

namespace fs = std::filesystem;

Page::Page(std::string base_path, std::string path) {
  base_path_ = base_path;
  fs::path p = path;
  p.replace_extension("");
  std::string new_path = p;
  path_ = new_path;
  name_ = path_.substr(path_.rfind("/")+1);
  GenerateParents();
}
    
void Page::GenerateParents() {
  std::vector<std::string> path_elems = func::Split(path_.substr(base_path_.length()), "/");
  for (size_t i=2; i<path_elems.size()-2; i++) {
    std::string path;
    for (size_t j=0; j<i; j++) 
      path += path_elems[j] + "/";
    path.pop_back();
    parents_[path] = path_elems[i];
  }
}
