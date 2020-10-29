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

#include "user.h"
#include "nlohmann/json.hpp"

/**
 * class storing all users.
 * TODO (fux): replace this class with a database.
 */
class UserManager {
  public:
    /**
     * constructor loading all users stored on disc.
     */
    UserManager(std::string path, std::vector<std::string> categories);

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
     * @brief checking password strength
     * Either 15 characters long, or 8 characters + 1 lowercase + 1 digit.
     * @param password (given password to check)
     * @return whether strength is sufficient.
     */
    bool CheckPasswordStrength(std::string password) const;

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
    
  private:
    std::map<std::string, User*> users_;  ///< Map of all users.
    mutable std::shared_mutex shared_mutex_users_;
    std::map<std::string, std::string> cookies_; ///< Map of cookies and users
    mutable std::shared_mutex shared_mutex_cookies_;
    const std::string path_;
    const std::vector<std::string> categories_;

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
};

#endif

