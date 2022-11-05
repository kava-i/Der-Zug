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
#include <inja/inja.hpp>

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
    User(std::string username, std::string pw, std::string path);

    /**
     * Constructor creating a (new) user.
     * @param[in] username.
     * @param[in] pw (password).
     * @param[in] path (path to folders).
     * @param[in] locations (locations available for this user).
     * @param[in] categories (all currently available categories)
    */
    User(std::string username, std::string pw, std::string path, 
        std::vector<std::string> locations);

    // ** getter ** //
    const std::string username() const;
    std::string password() const;
    std::map<std::string, int>& worlds();
    const std::vector<std::string>& locations() const;

    // ** setter ** //
    void set_password(std::string password);

    /**
     * Add a new location.
     * Adds a new location (a new world) to list of locations by adding path:
     * "[new_username]/files/[new_world]" to locations.
     * @param[in] user (user who granted access)
     * @param[in] world (new world which is added)
     */
    void AddLocation(std::string user, std::string world);

    // ** Serve an generate pages ** //
    
    /**
     * Gets all worlds and parses them into the overview page.
     * @param[in] shared_worlds (json with all shared_worlds and ports)
     * @param[in] all_worlds (json with all worlds and ports)
     * @param[in] textad_port (port of textadventure server)
     * @return overview-page.
     */
    std::string GetOverview(nlohmann::json shared_worlds, nlohmann::json all_worlds, int textad_port);

    /**
     * Get Overview of a backups. 
     * (f.e. world1/rooms = trainstation, hospital...)
     * @param[in] user
     * @param[in] world
     * @return overview page of all backups
     */
    std::string GetBackups(std::string user, std::string world);

    // ** Create New Files/ Folders ** //

    
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
     * @param[in] user 
     * @param[in] world 
     * @return ErrorCode.
     */
    int CreateBackup(std::string user, std::string world);

    /**
     * Restores a backup of given world.
     * @param[in] user 
     * @param[in] backup (given backup to restore)
     * @return ErrorCode.
     */
    int RestoreBackup(std::string user, std::string backup);

    /**
     * Delete a backup of given world.
     * @param[in] user 
     * @param[in] backup 
     * @return ErrorCode.
     */
    int DeleteBackup(std::string user, std::string backup);

    /**
     * Tries to run game.
     * Extracts user and world from path and tries to run game with all existing
     * players.
     * @param[in] path (path to extract information from)
     * @param[in] number (of players to check)
     * @return boolean indicating success.
     */
    bool CheckGameRunning(std::string path);

    /**
     * Adds a new access-request.
     * @param[in] user (user who asks for access)
     * @param[in] world (world to gran access for)
     * @return success.
     */
    bool AddRequest(std::string user, std::string world);

    /**
     * Removes an access-request.
     * @param[in] user (user who asks for access)
     * @param[in] world (world to gran access for)
     * @return success.
     */
    void RemoveRequest(std::string user, std::string world);

  private:
    const std::string username_;  ///< username (should be unique!)
    std::string password_;  ///< password 
    mutable std::shared_mutex shared_mtx_password_;
    const std::string path_;
    std::vector<std::string> locations_;
    mutable std::shared_mutex shared_mtx_locations_;
    mutable std::shared_mutex shared_mtx_worlds_;
    std::vector<nlohmann::json> requests_;
    mutable std::shared_mutex shared_mtx_requests_;

    /**
     * Construct a json with all the values from user.
     * @return return json of user.
     */
    nlohmann::json ConstructJson() const;
};

#endif
