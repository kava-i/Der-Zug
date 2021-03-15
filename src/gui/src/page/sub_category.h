/**
 * @author fux
*/

#ifndef SRC_SERVER_WORLD_SUBCATEGORY_H
#define SRC_SERVER_WORLD_SUBCATEGORY_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

#include "page/category.h"
#include "page/page.h"
#include "util/func.h"


/**
 * Class for all subcategories.
 * Subcategory means, that is children are all areas.
 */
class SubCategory : public Category {
  public: 
    // constructer/ destructor
    SubCategory(std::string base_path, std::string path) : Category(base_path, path) {}
    ~SubCategory() {}

    /**
     * Add new element:  directory, file or object.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @param[in] infos f.e name of start-room, when adding player.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes AddElem(std::string path, std::string name, nlohmann::json infos); 

    /**
     * Delete element:  directory, file or object.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes DelElem(std::string, std::string);

    /**
     * Restore deleted object.
     * Before deleting object, the object will be stored, and can be recreated.
     */
    ErrorCodes RestoreBackupObj(); 

  private:
    // private members:
    nlohmann::json last_deleted_area_;
    std::string path_to_last_deteled_area_;
};

#endif
