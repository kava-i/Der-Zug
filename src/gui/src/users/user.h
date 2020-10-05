/**
 * @author fux
*/

#ifndef SRC_SERVER_USER_H_
#define SRC_SERVER_USER_H_

#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include <stdlib.h>

class User {
  public:

    /**
     * Constructor creating a (new) user.
     * @param[in] username (name of user).
     * @param[in] user (json with user-data).
    */
    User(std::string username, std::string pw, std::string path, 
        std::vector<std::string> categories);

    // ** getter ** //
    std::string password() const;

    // ** setter ** //
    void set_password(std::string password);

    // ** Serve an generate pages ** //
    
    /**
     * Gets all words and parses them into the overview page.
     * @return overview-page.
     */
    std::string GetOverview();

    /**
     * Get Overview of one world. (f.e. world1)
     */
    std::string GetWorld(std::string world);

    /**
     * Get Overview of a category. 
     * (f.e. world1/rooms = trainstation, hospital...)
     */
    std::string GetCategory(std::string world, std::string category);

    /**
     * Get Overview of a SubCategory.
     * (f.e. world1/rooms/trainstation = platform a, great_hall ...)
     */
    std::string GetObjects(std::string world, std::string category, 
        std::string sub);

    /*
     * Get one object.
     */
    std::string GetObject(std::string world, std::string category, 
        std::string sub, std::string obj);


    // ** functions ** //

    /**
     * Write users jsons to disc.
     */
    void SafeUser() const;

  private:
    const std::string username_;  ///< username (should be unique!)
    std::string password_;  ///< password 
    mutable std::shared_mutex shared_mtx_password_;
    const std::string path_;
    const std::vector<std::string> categories_;
    /**
     * Construct a json with all the values from user.
     * @return return json of user.
     */
    nlohmann::json ConstructJson() const;
};

#endif
