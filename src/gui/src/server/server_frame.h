/**
 * @author fux
 */

#ifndef SERVER_SRC_SERVER_SERVERFRAME_H_
#define SERVER_SRC_SERVER_SERVERFRAME_H_

#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>

#include <httplib.h>

#include "users/user_manager.h"

class ServerFrame {
  public: 

    //Constructor
    ServerFrame();
  
    //Handler

   /**
    * Function to send login page, or overview page, depending on status:
    * logged in/ not logged in.
    * @param[in] req (reference to request)
    * @param[in, out] resp (server response)
    */
    void LoginPage(const httplib::Request& req, httplib::Response& resp) const;

   /**
    * Function to send overview page, redirect to login depending on status:
    * logged in/ not logged in.
    * @param[in] req (reference to request)
    * @param[in, out] resp (server response)
    */
    void Overview(const httplib::Request& req, httplib::Response& resp) const;

   /**
    * Function to send world page, redirect to login depending on status:
    * logged in/ not logged in.
    * f.e. world1, world2
    * @param[in] req (reference to request)
    * @param[in, out] resp (server response)
    */
    void World(const httplib::Request& req, httplib::Response& resp) const;

   /**
    * Function to send category page, redirect to login depending on status:
    * logged in/ not logged in.
    * f.e. attacks, config, ...
    * @param[in] req (reference to request)
    * @param[in, out] resp (server response)
    */
    void Category(const httplib::Request& req, httplib::Response& resp) const;

    /**
    * Function to send sub_catorypage, redirect to login depending on status:
    * logged in/ not logged in.
    * f.e. bathroom, corridor...
    * @param[in] req (reference to request)
    * @param[in, out] resp (server response)
    */
    void SubCategory(const httplib::Request& req, httplib::Response& resp) const;

    /**
    * Function to send details of one object
    * @param[in] req (reference to request)
    * @param[in, out] resp (server response)
    */
    void Object(const httplib::Request& req, httplib::Response& resp) const;

    /**
     * function to handle login
     * @param[in] req (reference to request)
     * @param[in, out] resp (server response)
     * @param[in] user_manager (user_manager to check whether user exists)
     */
    void DoLogin(const httplib::Request& req, httplib::Response& resp);

    /**
     * function to handle registration 
     * @param[in] req (reference to request)
     * @param[in, out] resp (server response)
    */
    void DoRegistration(const httplib::Request& req, httplib::Response& resp);

    /**
     * logout a user
     * @param[in] req (reference to request)
     * @param[in, out] resp (server response)
     */
    void DoLogout(const httplib::Request& req, httplib::Response& resp);

    /**
     * @brief Deletes a user. User is deleted on own request.
     * @param[in] req (reference to request)
     * @param[in, out] resp (reference to response)
     */
    void DelUser(const httplib::Request& req, httplib::Response& resp);

  private: 

    //Member
    UserManager user_manager_;  ///< currently class, later database with all users
    mutable std::shared_mutex shared_mtx_user_manager_;
};

#endif

