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
#include <ostream>
#include <regex>
#include <string>

using namespace httplib;


//Constructor
ServerFrame::ServerFrame(std::string path_to_cert, std::string path_to_key) 
  : user_manager_("../../data/users", {"attacks", 
    "defaultDialogs", "dialogs", "players", "rooms", "characters", 
    "defaultDescriptions", "details", "items", "quests", "texts", "images"})
#ifdef _COMPILE_FOR_SERVER_
    , server_(path_to_cert.c_str(), path_to_key.c_str())
#endif
{}

void ServerFrame::Start(int port) {
  std::cout << "Starting on Port: " << port << std::endl;

  // *** Pages *** //
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

  // *** Actions *** //

  //usermanagerment 
  server_.Post("/api/user_login", [&](const Request& req, Response& resp) {
      DoLogin(req, resp); });
  server_.Post("/api/user_registration", [&](const Request& req, Response& resp) {
      DoRegistration(req, resp); });
  server_.Post("/api/user_delete", [&](const Request& req, Response& resp) {
      DelUser(req, resp); });
  server_.Post("/api/user_logout", [&](const Request& req, Response& resp) {
      DoLogout(req, resp); });

  //Backups
  server_.Post("/api/create_backup", [&](const Request& req, Response& resp) {
      Backups(req, resp, "create"); });
  server_.Post("/api/restore_backup", [&](const Request& req, Response& resp) {
      Backups(req, resp, "restore"); });
  server_.Post("/api/delete_backup", [&](const Request& req, Response& resp) {
      Backups(req, resp, "delete"); });

  //Adding/ writing/ deleting
  server_.Post("/api/add_(.*)", [&](const Request& req, Response& resp) {
      AddElem(req, resp); });
  server_.Post("/api/write_object", [&](const Request& req, Response& resp) {
      WriteObject(req, resp); });
  server_.Post("/api/delete_(.*)", [&](const Request& req, Response& resp) {
      DelElem(req, resp); });

  //Access-rights
  server_.Post("/api/grant_access_to", [&](const Request& req, Response& resp) {
      GrantAccessTo(req, resp); });
  server_.Post("/api/create_request",  [&](const Request& req, Response& resp) {
      CreateRequest(req, resp); });

  //Running and testing game.
  server_.Post("/api/check_running", [&](const Request& req, Response& resp) {
      CheckRunning(req, resp); });
  server_.Post("/api/get_(.*)_log", [&](const Request& req, Response& resp) {
      GetLog(req, resp); });
  server_.Post("/api/start_game", [&](const Request& req, Response& resp) {
      StartGame(req, resp); });

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
  server_.Get("/web/run.js", [](const Request& req, Response& resp)
      { resp.set_content(func::GetPage("web/run.js"), 
          "application/javascript");});
  server_.Get("/web/fuzzy_finder.js", [](const Request& req, Response& resp)
      { resp.set_content(func::GetPage("web/fuzzy_finder.js"), 
          "application/javascript");});

  //Images
  server_.Get("/web/background.jpg", [](const Request& req, Response& resp) {
      resp.set_content(func::GetImage("web/images/background.jpg"), "image/jpg");});
  server_.Get("/web/logo.png", [](const Request& req, Response& resp) {
      resp.set_content(func::GetImage("web/images/logo.png"), "image/png");});


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

void ServerFrame::DoLogin(const Request& req, Response& resp) {
  nlohmann::json input = ValidateJson(req, resp, {"username", "password"});
  if (input.empty()) return;

  //Call DoLogin, returns "&msg: ..." in case of failure
  std::unique_lock ul(shared_mtx_user_manager_);
  std::string error = user_manager_.DoLogin(input["username"], input["password"]);
  ul.unlock();
  
  if (error != "") 
    resp.status = 401;
  else {
    //std::string cookie = "SESSID=" + GenerateCookie(username) + "; Path=/; SECURE";
    ul.lock();
    std::string cookie = "SESSID=" + user_manager_.GenerateCookie(input["username"])
      + "; Path=/";
    ul.unlock();
    resp.set_header("Set-Cookie", cookie.c_str());
  }
}

void ServerFrame::DoRegistration(const Request& req, Response& resp) {
  nlohmann::json input = ValidateJson(req, resp, {"id", "pw1", "pw2"});
  if (input.empty()) return;

  //Try to register user. Send error code if not possible.
  std::unique_lock ul(shared_mtx_user_manager_);
  std::string error = user_manager_.DoRegistration(input["id"], input["pw1"], input["pw2"]);
  ul.unlock();

  if (error != "") {
    resp.status = 401;
    resp.set_content(error, "application/json");
  }
  //If exists, log user in, by adding cookie.
  else {
    ul.lock();
    std::string cookie = "SESSID=" + user_manager_.GenerateCookie(input["id"]) 
      + "; Path=/";
    ul.unlock();
    resp.set_header("Set-Cookie", cookie.c_str());
    std::cout << input["id"] << " registered.\n";
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
  // Get cookie and try to get username from usermanager.
  std::string username = CheckLogin(req, resp);
  if (username == "") return;

  // Delete user.
  std::unique_lock ul(shared_mtx_user_manager_);
  user_manager_.DeleteUser(username);
  ul.unlock();
  resp.status=200;
}
void ServerFrame::ServeFile(const Request& req, Response& resp, bool backup) 
    const {
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;

  std::shared_lock sl(shared_mtx_user_manager_);
  User* user = user_manager_.GetUser(username);
  sl.unlock();

  if (user->CheckAccessToLocations(req.matches[0]) == false &&
      req.matches.size() > 1) {
    resp.status = 302;
    resp.set_header("Location", "/overview");
  }
  else {
    std::string page = "";
    try {
      if (req.matches.size() == 1) {
        sl.lock();
        nlohmann::json shared_worlds = user_manager_.GetSharedWorlds(username);
        nlohmann::json all_worlds = user_manager_.GetAllWorlds(username);
        sl.unlock();
        page = user->GetOverview(shared_worlds, all_worlds);
      }
      else {
        sl.lock();
        page = user_manager_.worlds()->GetPage(req.matches[0]);
        sl.unlock();
      } 
    } catch (std::exception& e) {
      std::cout << "ServeFile: " << e.what() << std::endl;
    }
    resp.set_content(page.c_str(), "text/html");
  }
}

void ServerFrame::AddElem(const Request& req, Response& resp) {
  std::cout << "AddElem()" << std::endl;
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;

  //Try to parse json
  nlohmann::json input = ValidateJson(req, resp, {"name"});
  std::string name = input["name"];
  std::string path = input.value("path", "");
  bool force = input.value("force", false);

  //Get user
  std::shared_lock sl(shared_mtx_user_manager_);
  User* user = user_manager_.GetUser(username);
  sl.unlock();

  int error_code = ErrorCodes::WRONG_FORMAT;
  //Check whether user has access to this location
  if (user->CheckAccessToLocations(path) == false &&
      req.matches.size() > 1 && req.matches[1] != "world") {
    error_code = ErrorCodes::ACCESS_DENIED;
  }
  else if (req.matches.size() > 1 && req.matches[1] == "world") {
    std::cout << "Calling CreateNewWorld()" << std::endl;
    error_code = user->CreateNewWorld(name, user_manager_.GetNextPort());
  }
  else {
    std::cout << "Calling user_manager_.worlds()->AddElem(...)" << std::endl;
    error_code = user_manager_.worlds()->AddElem(path, name, force);
  }

  //Check whether action succeeded
  if (error_code == ErrorCodes::SUCCESS) 
    resp.status = 200;
  else 
    resp.status = 401;
  resp.set_content(std::to_string(error_code), "text/txt");
}

void ServerFrame::DelElem(const Request& req, Response& resp) {
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;
  
  //Try to parse json
  nlohmann::json input = ValidateJson(req, resp, {"name"});
  if (input.empty()) return;
  std::string name = input["name"];
  std::string path = input.value("path", "");
  bool force = input.value("force", false);

  //Get user
  std::shared_lock sl(shared_mtx_user_manager_);
  User* user = user_manager_.GetUser(username);
  sl.unlock();

  int error_code = ErrorCodes::WRONG_FORMAT;
  //Check whether user has access to this location
  if (user->CheckAccessToLocations(path) == false &&
      req.matches.size() > 1 && req.matches[1] != "world") {
    error_code = ErrorCodes::ACCESS_DENIED;
  }
  else if (req.matches.size() > 1 && req.matches[1] == "world")
    error_code = user->DeleteWorld(name);
  else {
    std::cout << "Calling user_manager_.worlds()->DelElem(...)" << std::endl;
    error_code = user_manager_.worlds()->DelElem(path, name, force);
  }
  
  //Check whether action succeeded
  if (error_code == ErrorCodes::SUCCESS) 
    resp.status = 200;
  else 
    resp.status = 401;
  resp.set_content(std::to_string(error_code), "text/txt");
}

void ServerFrame::WriteObject(const Request& req, Response& resp) {
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;
  
  std::shared_lock sl(shared_mtx_user_manager_);
  //Call matching function.
  int error_code = user_manager_.GetUser(username)->WriteObject(req.body);
  sl.unlock();
  if (error_code == ErrorCodes::SUCCESS)
    resp.status = 200;
  else
    resp.status = 401;
  resp.set_content(std::to_string(error_code), "text/txt");
}

void ServerFrame::Backups(const Request& req, Response& resp, std::string action) {
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;
  
  // Parse from json
  nlohmann::json input = ValidateJson(req, resp, {"user"});
  if (input.empty()) return;
  std::string user = input["user"];
  std::string world = input.value("world", "");
  std::string backup = input.value("backup", "");
  std::string path = "/" + user + "/files/" + input.value("world", input.value("backup", "!!!!!"));

  std::shared_lock sl(shared_mtx_user_manager_);
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

void ServerFrame::GrantAccessTo(const Request& req, Response& resp) {
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;
  
  // Parse from json
  nlohmann::json input = ValidateJson(req, resp, {"user","world"});
  if (input.empty()) return;

  std::shared_lock sl(shared_mtx_user_manager_);
  int error_code = user_manager_.GrantAccessTo(username, input["user"], input["world"]);
  if (error_code == ErrorCodes::SUCCESS)
    resp.status = 200;
  else
    resp.status = 401;
  resp.set_content(std::to_string(error_code), "text/txt");
}

void ServerFrame::CreateRequest(const Request& req, Response& resp) {
  // Try to get username from cookie.
  std::string username = CheckLogin(req, resp);
  if (username == "") return;

  // Parse from json
  nlohmann::json input = ValidateJson(req, resp, {"user", "world"});
  if (input.empty()) return;

  std::shared_lock sl(shared_mtx_user_manager_);
  if (user_manager_.GetUser(input["user"])
      ->AddRequest(username, input["world"]) == false)
    resp.status = 401;
  else
    resp.status = 200;
}
 
void ServerFrame::CheckRunning(const Request& req, Response& resp) {
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;

  std::shared_lock sl(shared_mtx_user_manager_);
  bool success = user_manager_.GetUser(username)->CheckGameRunning(req.body);
  if (success == true) 
    resp.status = 200;
  else 
    resp.status = 401;
}

void ServerFrame::GetLog(const Request& req, Response& resp) {
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;

  size_t pos = req.body.find("/files/");
  std::string user = req.body.substr(1, req.body.find("/", 1));
  std::string world = req.body.substr(pos+7, req.body.find("/", pos+7)-(pos+7));
  std::string path = "../../data/users/" + user + "logs/";
  std::string type = req.matches[1];
  path += world + "_" + type + ".txt";

  if (func::demo_exists(path) == false)
    resp.status = 401;

  else {
    resp.status = 200;
    resp.set_content(func::GetPage(path), "text/txt");
  }
}

void ServerFrame::StartGame(const Request& req, Response& resp) {
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;

  //Extract data from path and build path to textadventure.
  std::string user = req.body.substr(1, req.body.find("/", 1)-1);
  std::string world = req.body.substr(req.body.rfind("/")+1);
  std::string path_to_game = "../../data/users" + req.body + "/";
  if (!func::demo_exists(path_to_game)) {
    resp.status = 401;
    return;
  }

  //Get port
  std::shared_lock sl(shared_mtx_user_manager_);
  std::string port = std::to_string(user_manager_.GetPortOfWorld(user, world));
  sl.unlock();

  //Build command and start game
  std::string command = "../../textadventure/build/bin/txtadventure "
    + path_to_game + " " + port
    + " > ../../data/users/" + user + "/logs/" + world + "_run.txt &";
  std::cout << system(command.c_str()) << std::endl;
  resp.status = 200;
}

std::string ServerFrame::CheckLogin(const Request& req, Response& resp) const { 
  //Try to get username from cookie
  const char* ptr = get_header_value(req.headers, "Cookie");
  std::shared_lock sl(shared_mtx_user_manager_);
  std::string username = user_manager_.GetUserFromCookie(ptr);
  sl.unlock();
  
  //If user does not exist, redirect to login-page.
  if (username == "") {
    resp.status = 302;
    resp.set_header("Location", "/login");
    return "";
  }
  return username;
}

nlohmann::json ServerFrame::ValidateJson(const Request& req, Response& resp, 
      std::vector<std::string> keys) const {
  try {
    nlohmann::json json = nlohmann::json::parse(req.body);
    for (auto it : keys) {
      std::string key = json[it];
      json[it] = key;
    }
    return json;
  }
  catch (std::exception& e) {
    resp.set_content("invalid json.", "text/txt");
    resp.status = 401;
    std::cout << "Error parsing request: " << e.what() << std::endl;
    return nlohmann::json();
  }
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
