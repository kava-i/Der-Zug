/**
 * @author fux
 */

#ifndef SRC_SERVER_WORLD_WORLDS_H
#define SRC_SERVER_WORLD_WORLDS_H

#include <filesystem>
#include <iostream>
#include <map>
#include <string>

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
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
     * Seves page of requested category/ object.
     * Acctually get json-data and path to tempate, then calls ParseTemplate().
     * @param[in] path (url)
     * @return rendered page.
     */
    std::string GetPage(std::string path);

  private:
    // member variables:
    int ports_;
    std::string base_path_;
    std::map<std::string, World*> worlds_;

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

