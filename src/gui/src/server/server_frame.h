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

    /**
     * Constructor. 
     * Creating user_manager with path to user-data and possible categories.
     */
    ServerFrame();

    /**
     * Starts server. 
     * Server is started on given port. Also handlers are initialized.
     * @param[in] port (Port to start server on)
     */
    void Start(int port);


    //Handler

    /**
     * Function to send login page, or overview page, depending on status:
     * logged in/ not logged in.
     * @param[in] req (reference to request)
     * @param[in, out] resp (server response)
     */
    void LoginPage(const httplib::Request& req, httplib::Response& resp) const;

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

    /**
      * Function to send all pages (except login and start page).
      * Redirect to login depending on status: logged in/ not logged in.
      * @param[in] req (reference to request)
      * @param[in, out] resp (server response)
      */
    void ServeFile(const httplib::Request& req, httplib::Response& resp,
        bool backup=false) const;

    /**
     * @brief Add new element (world, subcategory).
     * @param[in] req (reference to request)
     * @param[in, out] resp (reference to response)
     */
    void AddElem(const httplib::Request& req, httplib::Response& resp);

    /**
     * @brief Write (modified) object.
     * @param[in] req (reference to request)
     * @param[in, out] resp (reference to response)
     */
    void WriteObject(const httplib::Request& req, httplib::Response& resp);

    /**
     * @brief Creates a backup of the selected world.
     * @param[in] req (reference to request)
     * @param[in, out] resp (reference to response)
     */
    void Backups(const httplib::Request& req, httplib::Response& resp, 
        std::string action);

    /**
     * @brief Grants given user access to world of other user.
     * @param[in] req (reference to request)
     * @param[in, out] resp (reference to response)
     */
    void GrantAccessTo(const httplib::Request& req, httplib::Response& resp);

    /**    
     * @brief Gives feedback on whether server is still running    
     * @return boolean    
     */    
    bool IsRunning();    
    
    /**    
     * @brief Makes server stop running.    
     */    
    void Stop();    
    
    /**    
     * @brief Destructor, which stops server.    
     */    
    ~ServerFrame();

  private: 

    //Member
    httplib::Server server_;  //Server
    UserManager user_manager_;  ///< currently class, later database with all users
    mutable std::shared_mutex shared_mtx_user_manager_;
};

#endif

