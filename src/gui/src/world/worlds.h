/**
 * @author fux
 */

#ifndef SRC_SERVER_WORLD_WORLDS_H
#define SRC_SERVER_WORLD_WORLDS_H

#include <filesystem>
#include <iostream>
#include <map>
#include <shared_mutex>
#include <string>

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include <vector>
#include "util/error_codes.h"
#include "util/func.h"
#include "world.h"

/**
 * Class storing all existing worlds by it's full path.
 * Full path means: [base_path]/[user]/files/[world]
 * Apart from storing, the class redirects all requests to the matching world
 * and assignes port to every world, to potentially run on.
 */
class Worlds {
  public:
    // constructer/ destructor:
    /**
     * Constructor, loading all worlds.
     * @param[in] base_path to user-directories.
     * @param[in] start_port from which to increment and assign ports.
     */
    Worlds(std::string base_path, int start_port);

    /**
     * Destructor deleteing all worlds.
     */
    ~Worlds();

    // public methods:
    
    /** 
     * Create new world.
     * @param[in] path ([username]/files/[world_name])
     * @param[in] name of new world to create.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes CreateNewWorld(std::string path, std::string name);

    /**
     * Delete an existing world.
     * @param[in] path to world which shall be deleted.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes DeleteWorld(std::string path);

    /** 
     * Handles adding, deleting and modifiying elements.
     * @param[in] path to object, which shall be modified.
     * @param[in] name of object to add/ delete or payload of object which to modify.
     * @param[in] action might be "add", "delete" or "modify".
     * @param[in] force indicate whether to create althouh game might crash.
     * @param[in] obj json representation of an object. 
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes UpdateElements(std::string path, std::string name, std::string action, bool force, 
        nlohmann::json obj = nlohmann::json());
    
    /**
     * Serves page of requested category/ object.
     * Acctually get json-data and path to tempate, then calls ParseTemplate().
     * @param[in] path (url)
     * @return rendered page.
     */
    std::string GetPage(std::string path);

    /**
     * Serves only json of requested object.
     * @param[in] path (url)
     * @return json of object or empty json.
     */
    nlohmann::json GetObjectJson(std::string path);

    /**
     * Finds world with given url in all worlds.
     * Checks if world-path is contained in given base_path + (url-) path.
     * @param[in] path as url path.
     * @return world, or nullptr if not found.
     */
    World* GetWorldFromUrl(std::string path);

  private:
    // member variables:
    int ports_;
    std::string base_path_;
    std::map<std::string, World*> worlds_;
    mutable std::shared_mutex shared_mtx_worlds_;
    static const std::vector<std::string> categories_;

    // privave methods:
    
    /**
     * Parse template.
     * Loads template at given path and generates string with given data.
     * @param[in] json with "path" to template and "header" as data-object.
     * @return rendered page.
     */
    std::string ParseTemplate(nlohmann::json json);
};

#endif

