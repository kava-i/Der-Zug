/**
 * @author fux
*/

#ifndef SRC_SERVER_WORLD_CATEGORY_H
#define SRC_SERVER_WORLD_CATEGORY_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include "util/error_codes.h"
#include "util/func.h"

#include "page.h"

/**
 * Class for all categories.
 * As category is a page refering to other categories not objects.
 */
class Category : public Page {
  public: 
    // constructer/ destructor
    Category(std::string base_path, std::string path);
    ~Category() {}

    // public methods:
    
    /**
     * Add new element:  directory, file or object.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes AddElem(std::string, std::string) { return ErrorCodes::FAILED; }

    /**
     * Delete element:  directory, file or object.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes DelElem(std::string, std::string) { return ErrorCodes::FAILED; }

    /**
     * Calls base-class function.
     ** @param[in] path to area or object.
     * @return json with information.
     */
    nlohmann::json CreatePageData(std::string path);

  private:
    // private members:
    std::string path_to_last_backup;

    // private methods:
    /**
     * Generates all child nodes.
     * Iterates of all subdirectories and creates child-entry.
     */
    void GenerateChildNodes();
};

#endif
