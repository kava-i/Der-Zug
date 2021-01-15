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
     * Calls base-class function.
     ** @param[in] path to area or object.
     * @return json with information.
     */
    nlohmann::json CreatePageData(std::string path);

  private:
    // private methods:

    /**
     * Generates all child nodes.
     * Iterates of all subdirectories and creates child-entry.
     */
    void GenerateChildNodes();
};

#endif
