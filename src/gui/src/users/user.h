/**
 * @author fux
*/

#ifndef SRC_SERVER_USER_H_
#define SRC_SERVER_USER_H_

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include "inja.hpp"

#include <stdlib.h>
#include <time.h>

class User {
  public:

    /**
     * Constructor creating a (new) user.
     * @param[in] username.
     * @param[in] pw (password).
     * @param[in] path (path to folders).
     * @param[in] categories (all currently available categories)
    */
    User(std::string username, std::string pw, std::string path, std::vector
        <std::string> categories);

    /**
     * Constructor creating a (new) user.
     * @param[in] username.
     * @param[in] pw (password).
     * @param[in] path (path to folders).
     * @param[in] locations (locations available for this user).
     * @param[in] categories (all currently available categories)
    */
    User(std::string username, std::string pw, std::string path, std::vector
        <std::string> locations, std::vector<std::string> categories);


    // ** getter ** //
    std::string password() const;

    // ** setter ** //
    void set_password(std::string password);

    // ** Serve an generate pages ** //
    
    /**
     * Gets all worlds and parses them into the overview page.
     * @return overview-page.
     */
    std::string GetOverview();

    /**
     * @brief Get Overview of categories in one world.
     * @param path (path to current world)
     * @param world (name of accessed world)
     * @return HTML page or empty string.
     */
    std::string GetWorld(std::string path, std::string world);

    /**
     * Get Overview of a category. 
     * (f.e. world1/rooms = trainstation, hospital...)
     */
    std::string GetCategory(std::string path, std::string world, 
        std::string category);

    /**
     * Get Overview of a backups. 
     * (f.e. world1/rooms = trainstation, hospital...)
     */
    std::string GetBackups(std::string world);

    /**
     * Get Overview of a SubCategory.
     * (f.e. world1/rooms/trainstation = platform a, great_hall ...)
     */
    std::string GetObjects(std::string path, std::string world, std::string 
        category, std::string sub);

    /*
     * Get one object.
     */
    std::string GetObject(std::string path, std::string world, std::string 
        category, std::string sub, std::string obj);

    // ** Create New Files/ Folders ** //

    /**
     * @brief Creates a new world for this user
     * @param name (Name of the world)
     * @return Success code.
     */
    std::string CreateNewWorld(std::string name); 

    /**
     * @brief Adds new file to category
     * Adds new file and tries to create json matching category. 
     * @param path
     * @param name
     * @return error code.
     */
    std::string AddFile(std::string path, std::string name);
    

    /**
     * @brief Adds a new empty object
     * @param path
     * @param id
     * @return error code.
     */
    std::string AddNewObject(std::string path, std::string id);

    /**
     * Write json to disc.
     * @param[in] world (given world)
     * @return boolean to indicate success.
     */
    bool WriteObject(std::string request);


    // ** functions ** //
    
    /**
     * Write users jsons to disc.
     */
    void SafeUser() const;

    /**
     * Check whether user has access to given path.
     * @param[in] path.
     */
    bool CheckAccessToLocations(std::string path);

    /**
     * Create a backup of given world.
     * @param[in] world (given world)
     * @return boolean to indicate success.
     */
    bool CreateBackup(std::string world);

    /**
     * Restores a backup of given world.
     * @param[in] world (given world)
     * @return boolean to indicate success.
     */
    bool RestoreBackup(std::string backup);

    /**
     * Delete a backup of given world.
     * @param[in] world (given world)
     * @return boolean to indicate success.
     */
    bool DeleteBackup(std::string backup);


  private:
    const std::string username_;  ///< username (should be unique!)
    std::string password_;  ///< password 
    mutable std::shared_mutex shared_mtx_password_;
    const std::string path_;
    std::vector<std::string> locations_;
    mutable std::shared_mutex shared_mtx_locations_;
    const std::vector<std::string> categories_;

    /**
     * Construct a json with all the values from user.
     * @return return json of user.
     */
    nlohmann::json ConstructJson() const;
};

#endif
