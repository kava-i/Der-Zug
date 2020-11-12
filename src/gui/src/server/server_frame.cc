/**
 * @author georgbuechner
 */

#include "server_frame.h"
#include "users/user.h"
#include "util/error_codes.h"
#include "util/func.h"

#include "httplib.h"
#include "nlohmann/json.hpp"

#include <exception>
#include <iostream>
#include <string>

using namespace httplib;


//Constructor
ServerFrame::ServerFrame() : user_manager_("../../data/users", {"attacks", 
    "defaultDialogs", "dialogs", "players", "rooms", "characters", 
    "defaultDescriptions", "details", "items", "quests", "texts"})
{}

void ServerFrame::Start(int port) {
  std::cout << "Starting on Port: " << port << std::endl;

  //Pages
  server_.Get("/login", [&](const Request& req, Response& resp) { 
      LoginPage(req, resp);});
  server_.Get("/overview", [&](const Request& req, Response& resp) { 
      ServeFile(req, resp); });
  server_.Get("/(.*)/files/(.*)/(.*)/(.*)/(.*)", [&](const Request& req, 
        Response& resp) { 
      ServeFile(req, resp);});
  server_.Get("/(.*)/files/(.*)/(.*)/(.*)", [&](const Request& req, 
        Response& resp) { 
      ServeFile(req, resp); });
  server_.Get("/(.*)/files/(.*)/(.*)", [&](const Request& req, Response& resp) { 
      ServeFile(req, resp); });
  server_.Get("/(.*)/backups/(.*)", [&](const Request& req, Response& resp) { 
      ServeFile(req, resp, true); });
  server_.Get("/(.*)/files/(.*)", [&](const Request& req, Response& resp) { 
      ServeFile(req, resp); });

  //Actions
  server_.Post("/api/user_login", [&](const Request& req, Response& resp) {
      DoLogin(req, resp); });
  server_.Post("/api/user_registration", [&](const Request& req, Response& resp) {
      DoRegistration(req, resp); });
  server_.Post("/api/user_delete", [&](const Request& req, Response& resp) {
      DelUser(req, resp); });
  server_.Post("/api/user_logout", [&](const Request& req, Response& resp) {
      DoLogout(req, resp); });
  server_.Post("/api/create_backup", [&](const Request& req, Response& resp) {
      Backups(req, resp, "create"); });
  server_.Post("/api/restore_backup", [&](const Request& req, Response& resp) {
      Backups(req, resp, "restore"); });
  server_.Post("/api/delete_backup", [&](const Request& req, Response& resp) {
      Backups(req, resp, "delete"); });
  server_.Post("/api/add_(.*)", [&](const Request& req, Response& resp) {
      AddElem(req, resp); });
  server_.Post("/api/write_object", [&](const Request& req, Response& resp) {
      WriteObject(req, resp); });

  //html
  server_.Get("/", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/main.html"), "text/html"); });
  server_.Get("/login", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/login.html"), "text/html"); });
  server_.Get("/registration", [](const Request& req, Response& resp)
      { resp.set_content(func::GetPage("web/registration.html"), "text/html"); });

  //javascript/ css
  server_.Get("/web/general.css", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/general.css"), "text/css"); });
  server_.Get("/web/object.css", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/object.css"), "text/css"); });
  server_.Get("/web/general.js", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/general.js"), "application/javascript"); });
  server_.Get("/web/object.js", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/object.js"), "application/javascript"); });
  server_.Get("/web/backup.js", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/backup.js"), "application/javascript"); });
  server_.Get("/web/registration.js", [](const Request& req, Response& resp)
      { resp.set_content(func::GetPage("web/registration.js"), 
          "application/javascript");});
  server_.Get("/web/login.js", [](const Request& req, Response& resp)
      { resp.set_content(func::GetPage("web/login.js"), 
          "application/javascript");});
  server_.Get("/web/overview.js", [](const Request& req, Response& resp)
      { resp.set_content(func::GetPage("web/overview.js"), 
          "application/javascript");});

  //Images
  server_.Get("/web/background.jpg", [](const Request& req, Response& resp) {
      resp.set_content(func::GetImage("web/images/background.jpg"), "image/jpg");});

  std::cout << "C++ Api server startup successfull!" << std::endl;
  server_.listen("0.0.0.0", port);
}


//Handler

void ServerFrame::LoginPage(const Request& req, Response& resp) const {

  //Try to get username from cookie
  const char* ptr = get_header_value(req.headers, "Cookie");
  std::shared_lock sl(shared_mtx_user_manager_);
  std::string username = user_manager_.GetUserFromCookie(ptr);
  sl.unlock();

  //If user exists, redirect to /overview
  if (username != "") {
    resp.status = 302;
    resp.set_header("Location", "/overview");
  }
  else
    resp.set_content(func::GetPage("web/login.html"), "text/html");
}

void ServerFrame::ServeFile(const Request& req, Response& resp, bool backup) 
    const {
  //Try to get username from cookie
  const char* ptr = get_header_value(req.headers, "Cookie");
  std::shared_lock sl(shared_mtx_user_manager_);
  std::string username = user_manager_.GetUserFromCookie(ptr);
  User* user = user_manager_.GetUser(username);
  sl.unlock();

  //If user does not exist, redirect to login-page.
  if (username == "") {
    resp.status = 302;
    resp.set_header("Location", "/login");
  }
  else if (user->CheckAccessToLocations(req.matches[0]) == false &&
      req.matches.size() > 1) {
    resp.status = 302;
    resp.set_header("Location", "/overview");
  }
  else {
    std::string page = "";
    try {
      if (req.matches.size() == 1)
        page = user->GetOverview();
      else if (req.matches.size() == 3 && backup == false)
        page = user->GetWorld(req.matches[0], req.matches[2]);
      else if (req.matches.size() == 3 && backup == true)
        page = user->GetBackups(req.matches[2]);
      else if (req.matches.size() == 4)
        page = user->GetCategory(req.matches[0], req.matches[2], 
            req.matches[3]);
      else if (req.matches.size() == 5)
        page = user->GetObjects(req.matches[0], req.matches[2], req.matches[3], 
            req.matches[4]);
      else if (req.matches.size() == 6) {
        page = user->GetObject(req.matches[0], req.matches[2], req.matches[3], 
            req.matches[4], req.matches[5]);
      }
    }
    catch (std::exception& e) {
      std::cout << "ServeFile: " << e.what() << std::endl;
    }
    resp.set_content(page.c_str(), "text/html");
  }
}


void ServerFrame::DoLogin(const Request& req, Response& resp) {
  std::string username = "", pw = "";
  try {
    nlohmann::json user_input = nlohmann::json::parse(req.body);
    username = user_input["username"];
    pw = user_input["password"];
  }
  catch(std::exception& e) {
    std::cout << "Login failed: " << e.what() << std::endl;
    resp.status = 401;
  }

  //Call DoLogin, returns "&msg: ..." in case of failure
  std::unique_lock ul(shared_mtx_user_manager_);
  std::string error = user_manager_.DoLogin(username, pw);
  ul.unlock();
  
  if (error != "") 
    resp.status = 401;
  else {
    //std::string cookie = "SESSID=" + GenerateCookie(username) + "; Path=/; SECURE";
    ul.lock();
    std::string cookie = "SESSID=" + user_manager_.GenerateCookie(username)
      + "; Path=/";
    ul.unlock();
    resp.set_header("Set-Cookie", cookie.c_str());
    std::cout << username << " logged in.\n";
  }
}

void ServerFrame::DoRegistration(const Request& req, Response& resp) {
  std::string username ="", pw1="", pw2="";
  try {
    nlohmann::json user_input = nlohmann::json::parse(req.body);
    username = user_input["id"];
    pw1 = user_input["pw1"];
    pw2 = user_input["pw2"];
  }
  catch (std::exception& e) {
    resp.set_content("Wrong format for username or password", "application/json");
    std::cout << "Registration failed: " << e.what() << std::endl;
    return;
  }

  //Try to register user. Send error code if not possible.
  std::unique_lock ul(shared_mtx_user_manager_);
  std::cout << "Registering user: " << username << std::endl;
  std::string error = user_manager_.DoRegistration(username, pw1, pw2);
  ul.unlock();

  if (error != "") {
    resp.status = 401;
    resp.set_content(error, "application/json");
  }
  //If exists, log user in, by adding cookie.
  else {
    ul.lock();
    std::string cookie = "SESSID=" + user_manager_.GenerateCookie(username) 
      + "; Path=/";
    ul.unlock();
    resp.set_header("Set-Cookie", cookie.c_str());
    std::cout << username << " registered.\n";
  }
}

void ServerFrame::DoLogout(const Request& req, Response& resp) {
  
  //Get cookie and try to erase cookie from map in usermanager.
  const char* ptr = get_header_value(req.headers, "Cookie");
  std::shared_lock sl(shared_mtx_user_manager_);
  if (!user_manager_.DoLogout(ptr))
    std::cout << "Erasing cookie failed: cookie doesn't exist!\n";
  else   
    std::cout << "cookie from user erased. User logged out.\n";
}

void ServerFrame::DelUser(const Request& req, Response& resp) {
  //Get cookie and try to get username from usermanager.
  const char* ptr = get_header_value(req.headers, "Cookie");
  std::shared_lock sl(shared_mtx_user_manager_);
  std::string username = user_manager_.GetUserFromCookie(ptr);
  sl.unlock();
  //Sent controller value-page as respond.
  if (username != "") {
    std::unique_lock ul(shared_mtx_user_manager_);
    user_manager_.DeleteUser(username);
    ul.unlock();
    resp.status=200;
  }
  //If user couldn't be found, do nothing, redirect to login page.
  else {
    resp.status = 302;
    resp.set_header("Location", "/login");
    return;
  }
}

void ServerFrame::AddElem(const Request& req, Response& resp) {
  //Try to get username from cookie
  const char* ptr = get_header_value(req.headers, "Cookie");
  std::shared_lock sl(shared_mtx_user_manager_);
  std::string username = user_manager_.GetUserFromCookie(ptr);
  sl.unlock();

  //If user does not exist, redirect to login-page.
  if (username == "") {
    resp.status = 302;
    resp.set_header("Location", "/login");
    return;
  }
  
  //Try to parse json
  std::string name, path;
  try {
    nlohmann::json request = nlohmann::json::parse(req.body);
    name = request.value("world", request.value("subcategory", 
          request.value("name", "")));
    path = request.value("path", "");
  }
  catch (std::exception& e) {
    std::cout << "Parsing json failed: " << e.what() << std::endl;
    resp.status = 401;
    resp.set_content(std::to_string(ErrorCodes::WRONG_FORMAT), "text/txt");
    return;
  }
  
  //Get user
  sl.lock();
  User* user = user_manager_.GetUser(username);

  sl.unlock();
  int error_code = ErrorCodes::WRONG_FORMAT;
  //Check whether user has access to this location
  if (user->CheckAccessToLocations(path) == false &&
      req.matches.size() > 1 && req.matches[1] != "world") {
    error_code = ErrorCodes::ACCESS_DENIED;
  }
  else if (req.matches.size() > 1 && req.matches[1] == "world")
    error_code = user->CreateNewWorld(name);
  else if (req.matches.size() > 1 && req.matches[1] == "subcategory") 
    error_code = user->AddFile(path, name);
  else if (req.matches.size() > 1 && req.matches[1] == "object") 
    error_code = user->AddNewObject(path, name);

  //Check whether action succeeded
  if (error_code == ErrorCodes::SUCCESS) 
    resp.status = 200;
  else 
    resp.status = 401;
  resp.set_content(std::to_string(error_code), "text/txt");
}

void ServerFrame::WriteObject(const Request& req, Response& resp) {
  //Try to get username from cookie
  const char* ptr = get_header_value(req.headers, "Cookie");
  std::shared_lock sl(shared_mtx_user_manager_);
  std::string username = user_manager_.GetUserFromCookie(ptr);
  sl.unlock();

  //If user does not exist, redirect to login-page.
  if (username == "") {
    resp.status = 302;
    resp.set_header("Location", "/login");
  }
  else {
    sl.lock();
    //Call matching function.
    int error_code = user_manager_.GetUser(username)->WriteObject(req.body);
    sl.unlock();
    if (error_code == ErrorCodes::SUCCESS)
      resp.status = 200;
    else
      resp.status = 401;
    resp.set_content(std::to_string(error_code), "text/txt");
  }
}

void ServerFrame::Backups(const Request& req, Response& resp, std::string action) {
  //Try to get username from cookie
  const char* ptr = get_header_value(req.headers, "Cookie");
  std::shared_lock sl(shared_mtx_user_manager_);
  std::string username = user_manager_.GetUserFromCookie(ptr);
  sl.unlock();

  //If user does not exist, redirect to login-page.
  if (username == "") {
    resp.status = 302;
    resp.set_header("Location", "/login");
    return;
  }

  //Parse user, backup and world from request
  std::string user, world, backup, path;
  try {
    nlohmann::json json = nlohmann::json::parse(req.body);
    user = json["user"];
    world = json.value("world", "");
    backup = json.value("backup", "");
    path = "/"+user+"/files/"+json.value("world", json.value("backup", "!!!!!"));
  } 
  catch (std::exception& e) {
    std::cout << "Parsing values from request failed: " << e.what() << std::endl;
    resp.status = 401;
    resp.set_content(std::to_string(ErrorCodes::WRONG_FORMAT), "text/txt");
    return;
  }

  sl.lock();
  //Call matching function.
  int error_code = false;
  if (user_manager_.GetUser(username)->CheckAccessToLocations(path) == false)
    error_code = ErrorCodes::ACCESS_DENIED;
  else if (action == "create")
    error_code = user_manager_.GetUser(username)->CreateBackup(user, world);
  else if (action == "restore") 
    error_code = user_manager_.GetUser(username)->RestoreBackup(user, backup);
  else if (action == "delete")
    error_code = user_manager_.GetUser(username)->DeleteBackup(user, backup);
  else
    error_code = false;
  sl.unlock();

  if (error_code == ErrorCodes::SUCCESS)
    resp.status = 200;
  else
    resp.status = 401;
  resp.set_content(std::to_string(error_code), "text/txt");
}

bool ServerFrame::IsRunning() {
  return server_.is_running();
}

void ServerFrame::Stop() {
  server_.stop();
}

ServerFrame::~ServerFrame() {
  Stop();
}
