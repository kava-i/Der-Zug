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
    std::string name() const;
    std::string creator() const;

    // public methods:
    
    /**
     * Modify and existing object. Restores old object, if game is not running.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @param[in] modified_obj json payload of mofified object.
     * @param[in] force indicate whether to create althouh game might crash.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes ModifyObject(std::string path, std::string name, nlohmann::json modified_object, bool force);

    /**
     * Add new element:  directory, files or object.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @param[in] infos potential additional information (f.e. name of start-room).
     * @param[in] force indicate whether to create althouh game might crash.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes AddElem(std::string path, std::string name, nlohmann::json infos, bool force);

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
     * @param[in] only_json if true, then returns only the objects json.
     * @return json with page-data, path to template and short-paths.
     */
    nlohmann::json GetPage(std::string path, bool only_json=false) const;

    /**
     * Provides a graph representation if availibe and an empty json otherwise.
     * @return json with graph-representation of area.
     */
    nlohmann::json GetGraph(std::string path) const;

    /**
     * Provides the notes of a object.
     * @return string with notes.
     */
    nlohmann::json GetNotes(std::string path) const;

    void SetNotes(std::string path, std::string notes);


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

    /**
     * Makes changes of action (delete, add, modify element) permanent if game
     * is still running. Reverts changes otherwise.
     * @return Error-code (succes, or game not running).
     */
    ErrorCodes RevertIfGameNotRunning(std::string path, std::string id, std::string action);

    void RefreshWorld();
};

#endif
