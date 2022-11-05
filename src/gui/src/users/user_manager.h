/**
 * @author fux
*/

#ifndef SRC_SERVER_USERS_H_
#define SRC_SERVER_USERS_H_

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>

#include "nlohmann/json.hpp"

#include "user.h"
#include "world/worlds.h"

/**
 * class storing all users.
 * TODO (fux): replace this class with a database.
 */
class UserManager {
  public:
    /**
     * constructor loading all users stored on disc.
     */
    UserManager(std::string path);

    // getter:
    Worlds* worlds() const;

    /**
     * Returns given user, if exists
     * @param[in] username
     * @return user, if exists, nullptr otherwise
     */
    User* GetUser(std::string username) const;

    /**
     * detele user.
     * deletes object, erases from map and delete users jsons file.
     * @param[in] username
     */
    void DeleteUser(std::string username);

    /**
     * Logout user. 
     * try to get user from cookie. Logout user, reset controller update 
     * interval and erase cookie. If user does not exist, try only to delete 
     * cookie from map.
     * @param[in] cookie.
     * @return false if cookie does not exist in map.
     */
    bool DoLogout(const char* ptr);

    /**
     * Check whether login was successful. Return error if not. 
     * @param username
     * @param password
     * @return error or empty string
     */
    std::string DoLogin(std::string username, std::string password);
    
    /**
     * Check whether registration was successful. Return error or empty string. 
     * @param username
     * @param pw1
     * @param pw2
     * @return error code or empty string.
     */
    std::string DoRegistration(std::string username, std::string pw1, 
        std::string pw2);
    
    /**
     * Create random 32 characters to generates cookie. And maps cookie and given
     * user.
     * @param[in] username (username which is mapped on cookie)
     * @return returns cookie as string.
     */
    std::string GenerateCookie(std::string username);

    /**
     * Get user from cookie.
     * @param[in] resp (reference to response)
     * @return username, "$no_cookie" or "$no_user"
     */
    std::string GetUserFromCookie(const char* ptr) const;

    /**
     * Grant other user acces to a own world.
     * @param[in] user1 (user which gives access world to)
     * @param[in] user2 (user to give access to)
     * @param[in] world (world which to grant access to)
     * @return error_code
     */
    int GrantAccessTo(std::string user1, std::string user2, std::string world);

    /**
     * Returns next port.
     */
    int GetNextPort();

    /**
     * Get all worlds.
     * Array of jsons returned with values: user, name (of world) and port.
     * @param[in] user to create list for.
     * return json
     */
    nlohmann::json GetAllWorlds(std::string username) const;

    /**
     * Get all shared worlds of a user.
     * Array of jsons returned with values: user, name (of world) and port.
     * @param[in] user to create list for.
     * return json
     */
    nlohmann::json GetSharedWorlds(std::string username) const;

  private:
    std::map<std::string, User*> users_;  ///< Map of all users.
    mutable std::shared_mutex shared_mutex_users_;
    std::map<std::string, std::string> cookies_; ///< Map of cookies and users
    mutable std::shared_mutex shared_mutex_cookies_;
    const std::string path_;
    const std::vector<std::string> categories_;
    int ports_;
    mutable std::shared_mutex shared_mutex_ports_;

    Worlds* worlds_;

    /**
     * Adds new user
     * @param[in] username
     * @param[in] password
     */
    void AddUser(std::string username, std::string password);

    /**
     * Check if username and password match/ exist. Return error-code if not.
     * Return empty json if login succeeded. 
     * @param[in] username
     * @param[in] password
     * @return json with error code if no success.
     */
    nlohmann::json CheckLogin(std::string username, std::string password) const;

    /**
     * @brief checking password strength
     * Either 15 characters long, or 8 characters + 1 lowercase + 1 digit.
     * @param password (given password to check)
     * @return whether strength is sufficient.
     */
    bool CheckPasswordStrength(std::string password) const;
};

#endif

