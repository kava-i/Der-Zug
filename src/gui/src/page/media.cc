#include "media.h"
#include "util/error_codes.h"
#include <exception>
#include <filesystem>

namespace fs = std::filesystem;

Media::Media(std::string base_path_, std::string path, std::string type, std::string extension)
  : Page(base_path_, path), type_(type), extension_(extension) {
  category_ = (--parent_nodes_.rbegin())->second;

  std::ifstream read(base_path_ + "/" + path_ + "/" + name_ + ".md");
  if (read) {
    read >> notes_;
    read.close();
  }
}

ErrorCodes Media::DelElem(std::string path, std::string name) {
  std::cout << "Media::DelElem(" << path << ")" << std::endl;
  try {
    fs::remove(path);
  }
  catch (std::exception &e) {
    std::cout << "Failed removing element." << std::endl;
    return ErrorCodes::PATH_NOT_FOUND;
  }
  return ErrorCodes::SUCCESS;
}

nlohmann::json Media::CreatePageData(std::string path) {
  nlohmann::json json;
  json["path"] = "web/object_templates/media_template.html";
  json["header"] = nlohmann::json({{"name", name_}, {"type", type_}, {"extension", extension_},
      {"nodes", nlohmann::json::array()}, {"__parents", parent_nodes_}});
  return json;
}


