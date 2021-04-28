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
  : user_manager_("../../data/users")
#ifdef _COMPILE_FOR_SERVER_
    , server_(path_to_cert.c_str(), path_to_key.c_str())
#endif
{}

void ServerFrame::Start(int port) {
  std::cout << "Starting on Port: " << port << std::endl;
  
  // Get only json.
  server_.Get("/api/get_object", [&](const Request& req, Response& resp) {
      GetObjectInfo(req, resp); });

  // *** Pages *** //
 
  server_.Get("/login", [&](const Request& req, Response& resp) { 
      LoginPage(req, resp);});
  server_.Get("/overview", [&](const Request& req, Response& resp) { 
      ServeFile(req, resp); });
  server_.Get("/(.*)/files/(.*)/(.*)/(.*)/(.*)", [&](const Request& req, Response& resp) { 
      ServeFile(req, resp);});
  server_.Get("/(.*)/files/(.*)/(.*)/(.*)", [&](const Request& req, Response& resp) { 
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
  server_.Get("/web/cytoscape.min.js", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/cytoscape.min.js"), "application/javascript"); });
  server_.Get("/web/graph.js", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/graph.js"), "application/javascript"); });
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
  server_.Get("/favicon.png", [](const Request& req, Response& resp) {
      resp.set_content(func::GetImage("web/images/favicon.png"), "image/png");});


  std::cout << "C++ Api server startup successfull!\n" << std::endl;
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
    std::cout << input["id"] << " registered.\n" << std::endl;
  }
}

void ServerFrame::DoLogout(const Request& req, Response& resp) {
  //Get cookie and try to erase cookie from map in usermanager.
  const char* ptr = get_header_value(req.headers, "Cookie");
  std::shared_lock sl(shared_mtx_user_manager_);
  if (!user_manager_.DoLogout(ptr))
    std::cout << "Erasing cookie failed: cookie doesn't exist!\n" << std::endl;
  else   
    std::cout << "cookie from user erased. User logged out.\n" << std::endl;
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
        page = user->GetOverview(shared_worlds, all_worlds);
      }
      else {
        sl.lock();
        page = user_manager_.worlds()->GetPage(req.matches[0]);
      } 
    } catch (std::exception& e) {
      std::cout << "ServeFile: " << e.what() << std::endl;
    }
    resp.set_content(page.c_str(), "text/html");
  }
}

void ServerFrame::GetObjectInfo(const Request& req, Response& resp) {
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") {
    resp.status = 404;
    return;
  }

  // Get query parameters and return error code if query parameters are missing.
  if (!req.has_param("type") || !req.has_param("path")) {
    std::cout << "Missing parameters." << std::endl;
    resp.status = 400;
    return;
  }
  std::string path = req.get_param_value("path");
  std::string info_type = req.get_param_value("type");
  std::cout << "GetObjectJson(" << info_type << ", " << path << ")" << std::endl;

  //Get user
  std::shared_lock sl(shared_mtx_user_manager_);
  User* user = user_manager_.GetUser(username);
  sl.unlock();

  nlohmann::json object_json = nlohmann::json();
  //Check whether user has access to this location
  if (user->CheckAccessToLocations(path) == false &&
      req.matches.size() > 1 && req.matches[2] != "world") {
    resp.status = 401;
  }
  // Get Object Infos
  else {
    if (info_type == "json") {
      std::cout << "calling GetObjectJson" << std::endl;
      object_json = user_manager_.worlds()->GetObjectJson(path);
      if (!object_json.empty())
        resp.status = 200;
      else
        resp.status = 404;
    }
    else if (info_type == "graph") {
      std::cout << "calling Graph" << std::endl;
      object_json = user_manager_.worlds()->GetGraph(path);
    }
  }
  
  //Check whether action succeeded
  std::cout << "ServerFrame::GetObject" << info_type << " done with: " << object_json << ".\n\n";
  resp.set_content(object_json.dump(), "application/json");
}

void ServerFrame::AddElem(const Request& req, Response& resp) {
  std::cout << "ServerFrame::AddElem()" << req.body << std::endl;
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;

  //Try to parse json
  nlohmann::json input = ValidateJson(req, resp, {"name", "infos"});
  std::string name = input["name"];
  std::string path = input.value("path", "");
  nlohmann::json infos = input["infos"];
  bool force = input.value("force", false);

  std::cout << "Got infos: " << infos << std::endl;

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
  // Create new world
  else if (req.matches.size() > 1 && req.matches[1] == "world")
    error_code = user_manager_.worlds()->CreateNewWorld("/"+username+"/files/"+name, name);
  // Create new object or (sub-)category.
  else
    error_code = user_manager_.worlds()->UpdateElements(path, name, "add", force, infos);

  //Check whether action succeeded
  if (error_code == ErrorCodes::SUCCESS) 
    resp.status = 200;
  else 
    resp.status = 401;
  std::cout << "ServerFrame::AddElem done with error_code: " << error_code << "\n\n";
  resp.set_content(std::to_string(error_code), "text/txt");
}

void ServerFrame::DelElem(const Request& req, Response& resp) {
  std::cout << "ServerFrame::DelElem." << std::endl;
  std::cout << "Body: " << req.body << std::endl;
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;
  
  //Try to parse json
  nlohmann::json input = ValidateJson(req, resp, {"name", "path"});
  if (input.empty()) return;
  std::string name = input["name"];
  std::string path = input["path"];
  bool force = input.value("force", false);

  //Get user
  std::shared_lock sl(shared_mtx_user_manager_);
  User* user = user_manager_.GetUser(username);
  sl.unlock();

  int error_code = ErrorCodes::WRONG_FORMAT;
  //Check whether user has access to this location
  if (req.matches.size() > 1 && req.matches[1] == "world")
    error_code = user_manager_.worlds()->DeleteWorld("/"+username+"/files/"+name);
  else if (user->CheckAccessToLocations(path))
    error_code = user_manager_.worlds()->UpdateElements(path, name, "delete", force);
  else
    error_code = ErrorCodes::ACCESS_DENIED;
  
  // Construct response.
  if (error_code == ErrorCodes::SUCCESS) 
    resp.status = 200;
  else 
    resp.status = 401;
  std::cout << "ServerFrame::DelElem done with error_code: " << error_code << "\n\n";
  resp.set_content(std::to_string(error_code), "text/txt");
}

void ServerFrame::WriteObject(const Request& req, Response& resp) {
  std::cout << "ServerFrame::WriteObject." << std::endl;
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;
  
  //Try to parse json and get relevant attributes.
  nlohmann::json json_req = ValidateJson(req, resp, {"json/id", "path"});
  if (json_req.empty()) return;
  std::string id = json_req["json"]["id"];
  std::string path = json_req["path"];
  nlohmann::json modified_obj = json_req["json"];
  bool force = json_req.value("force", false);
  
  //Get user
  std::shared_lock sl(shared_mtx_user_manager_);
  int error_code;
  User* user = user_manager_.GetUser(username);
  sl.unlock();

  // Call function to modify object.
  if (user->CheckAccessToLocations(path))
    error_code = user_manager_.worlds()->UpdateElements(path, id, "modify", force, modified_obj);
  else 
    error_code = ErrorCodes::ACCESS_DENIED;

  // Construct response.
  if (error_code == ErrorCodes::SUCCESS)
    resp.status = 200;
  else
    resp.status = 401;
  std::cout << "ServerFrame::WriteObject done with error_code: " << error_code << "\n\n";
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
  std::cout << "ServerFrame::Backups done with error_code: " << error_code << "\n\n";
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
  std::cout << "ServerFrame::GrantAccessTo done with error_code: " << error_code << "\n\n";
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
  // Try to get username from cookie
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
  // Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;

  // Extract data from path and build path to textadventure.
  std::string url_path = req.body;
  std::string path_to_game = "../../data/users" + url_path + "/";
    
  // Get world (keep mutex locked to assure, that pointer is not changed. 
  std::shared_lock sl(shared_mtx_user_manager_);
  World* world = user_manager_.worlds()->GetWorldFromUrl(url_path);
  if (world == nullptr || !func::demo_exists(path_to_game)) {
    resp.status = 401;
    return;
  }

  // Build command and start game
  std::string command = "../../textadventure/build/bin/txtadventure "
    + path_to_game + " " + std::to_string(world->port())
    + " > ../../data/users/" + world->creator() + "/logs/" + world->name() + "_run.txt &";
  sl.unlock();

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
  nlohmann::json json = func::ValidateJson(req.body, keys);
  if (json.empty()) {
    resp.set_content("invalid json.", "text/txt");
    resp.status = 401;
    return nlohmann::json();
  }
  return json;
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
