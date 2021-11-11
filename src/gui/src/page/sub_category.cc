#include "sub_category.h"
#include "nlohmann/json.hpp"
#include "page/media.h"
#include "util/error_codes.h"
#include "util/func.h"
#include <exception>
#include <fstream>
#include <ostream>

namespace fs = std::filesystem;

ErrorCodes SubCategory::AddElem(std::string path, std::string name, nlohmann::json infos) {
  std::string path_to_elem = path_ + "/" + name + ".json";
  std::cout << "SubCategory::AddElem(" << path_to_elem << ")" << std::endl;
  // Check if element already exists.
  if (func::demo_exists(path_to_elem))
    return ErrorCodes::ALREADY_EXISTS;
  // Create new element from template, add to elements and write to disc.
  nlohmann::json new_area;
  if (!GetObjectsFromTemplate(new_area))
    return ErrorCodes::PATH_NOT_FOUND;
  std::cout << new_area << std::endl;
  func::WriteJsonToDisc(path_to_elem, new_area);
  return ErrorCodes::SUCCESS; 
}

ErrorCodes SubCategory::DelElem(std::string path, std::string name) {
  std::string path_to_elem = path_ + "/" + name + ".json";
  std::cout << "SubCategory::DelElem(" << path_to_elem << ")" << std::endl;
  nlohmann::json cur_area;
  if (!func::LoadJsonFromDisc(path_to_elem, cur_area))
    return ErrorCodes::PATH_NOT_FOUND;
  last_deleted_area_ = cur_area;
  path_to_last_deteled_area_ = path_to_elem;

  // Erase element from elements and write to disc.
  try {
    fs::remove(path_to_elem);
  } catch (std::exception& e) {
    std::cout << "Failed deleting area: " << e.what() << std::endl;
    return ErrorCodes::FAILED;
  }
  return ErrorCodes::SUCCESS; 
}

ErrorCodes SubCategory::RestoreBackupObj(std::string) {
  std::cout << "Area::UndoDelElem()" << std::endl;
  func::WriteJsonToDisc(path_to_last_deteled_area_, last_deleted_area_);
  return ErrorCodes::SUCCESS;
}
