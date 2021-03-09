/**
 * @author fux
*/

#ifndef SRC_SERVER_WORLD_WORLD_H
#define SRC_SERVER_WORLD_WORLD_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <shared_mutex>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

#include "page/area.h"
#include "page/category.h"
#include "page/page.h"
#include "page/sub_category.h"
#include "util/error_codes.h"
#include "util/func.h"

/**
 * Class storing all pages and information of a world.
  */
class World {
  public:
    // constructer/ destructor:
    World() : base_path_(""), path_(""), port_(0){} 

    /**
     * Constructor generating all pages.
     * @param[in] base_path to user-directory.
     */
    World(std::string base_path, std::string path, int port);
    
    /**
     * Destructor deleteing all pages.
     */
    ~World();

    // getter:
    int port() const;

    // public methods:
    
    /**
     * Add new element:  directory, files or object.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @param[in] force indicate whether to create althouh game might crash.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes AddElem(std::string path, std::string name, bool force);

    /**
     * Delete element:  directory, files or object.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @param[in] force indicate whether to create althouh game might crash.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes DelElem(std::string path, std::string name, bool force);

    /**
     * Calls GetPage value of referenced page and add short paths to data-json.
     * @param[in] path to category/area/object.
     * @return json with page-data, path to template and short-paths.
     */
    nlohmann::json GetPage(std::string path) const;

  private:
    // member variables:
    const std::string base_path_;
    const std::string path_;
    std::string name_;
    std::string creator_;
    const int port_; // port=http-server, port+1=websocketserver.

    /**
     * All pages are stored with the full path Category/Area as value.
     * - Categories: [base_path]/[user]/files/[world]/[path_to_directory]
     * - Areas: [base_path]/[user]/files/[world]/[path_to_directory without ".json"
     * - objects: [base_path]/[user]/files/[world]/[path_to_directory]/[object_id]
     */
    std::map<std::string, Page*> paths_;

    mutable std::shared_mutex shared_mtx_paths_;

    /**
     * Stores all paths as presented in url with name as value.
     * - Categories: [user]/files/[world]/[path_to_directory]
     * - Areas: [user]/files/[world]/[path_to_directory without ".json"
     * - objects: [user]/files/[world]/[path_to_directory]/[object_id]
     */
    std::vector<std::string> short_paths_;

    // privave methods:
    
    /**
     * Creates all pages.
     * @param[in] path of world.
     */
    void InitializePaths(std::string path);

    /**
     * Generate all "short paths", t.i.~paths as presented in url.
     */
    void UpdateShortPaths();

    /**
     * Checks if game is running. Runns game with basic tests.
     * @return boolean indicating success.
     */
    bool IsGameRunning() const;
    
    /**
     * Converts id to lower and replaces all spaces with underscores.
     * @param[in, out] id which is modified.
     */
    static void ConvertId(std::string& id);
};

#endif
