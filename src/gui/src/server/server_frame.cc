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
#include <memory>
#include <ostream>
#include <regex>
#include <sstream>
#include <string>

using namespace httplib;

// Constructor
ServerFrame::ServerFrame(int textad_port, std::filesystem::path base_path, 
    std::string path_to_cert, std::string path_to_key) 
  : textad_port_(textad_port), user_manager_(base_path), base_path_(base_path)
#ifdef _COMPILE_FOR_SERVER_
    , server_(path_to_cert.c_str(), path_to_key.c_str())
#endif
{ }

void ServerFrame::Start(int port) {
  std::cout << "Starting on Port: " << port << std::endl;
  
  // Get only json.
  server_.Get("/api/get_object", [&](const Request& req, Response& resp) {
      GetObjectInfo(req, resp); });

  server_.Post("/api/set_notes", [&](const Request& req, Response& resp) {
      SetNotes(req, resp); });


  // *** Pages *** //

  server_.Get("/(.*)/files/(.*)/images/(.*).jpg", [this](const Request& req, Response& resp) {
      std::filesystem::path path_to_image = base_path_ / req.matches[0].str().substr(1);
      resp.set_content(func::GetMedia(path_to_image), "image/jpeg");
  });
  server_.Get("/(.*)/files/(.*)/images/(.*).png", [this](const Request& req, Response& resp) {
      std::filesystem::path path_to_image = base_path_ / req.matches[0].str().substr(1);
      resp.set_content(func::GetMedia(path_to_image), "image/png");
  });


  server_.Get("/(.*)/files/(.*)/sounds/(.*).mp3", [this](const Request& req, Response& resp) {
      std::filesystem::path path_to_image = base_path_ / req.matches[0].str();
      resp.set_content(func::GetMedia(path_to_image), "audio/mp3");
  });

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

  // usermanagerment 
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
  server_.Post("/api/upload/images",  [&](const Request& req, Response& resp) {
      WriteMedia(req, resp, "image"); });
  server_.Post("/api/upload/sounds",  [&](const Request& req, Response& resp) {
      WriteMedia(req, resp, "audio"); });
  server_.Post("/api/edit-template/(.*)",  [&](const Request& req, Response& resp) {
      GetEditTemplate(req, resp, req.matches[1]); });


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
  server_.Post("/api/start", [&](const Request& req, Response& resp) {
      StartGame(req, resp); });
  server_.Post("/api/close", [&](const Request& req, Response& resp) {
      CloseGame(req, resp); });
  server_.Post("/api/running", [&](const Request& req, Response& resp) {
      IsGameRunning(req, resp); });

  //html
  server_.Get("/", [&](const Request& req, Response& resp) {
      resp.set_content(func::GetPage("web/main.html"), "text/html"); });
  server_.Get("/(.*).md", [&](const Request& req, Response& resp) {
      std::cout << "Serving info page... " << req.matches[1] << std::endl;
      // Load markdown and convert to html
      std::string markdown = func::LoadMarkdown("web/tutorials/" + (std::string)req.matches[1] + ".md");
      // Parse template to include markdown-html
      try {
        inja::Environment env;
        inja::Template temp = env.parse_template("web/info_template.html");
        std::string page = env.render(temp, {{"markdown", markdown}});
        std::cout << "parsed template..." << std::endl;
        // Set content
        resp.set_content(page, "text/html"); 
      } catch (std::exception& e) {
        std::cout << "Problem parsing object html: " << e.what() << "\n\n";
      }
  });
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
      resp.set_content(func::GetMedia("web/images/background.jpg"), "image/jpg");});
  server_.Get("/web/logo.png", [](const Request& req, Response& resp) {
      resp.set_content(func::GetMedia("web/images/logo.png"), "image/png");});
  server_.Get("/favicon.png", [](const Request& req, Response& resp) {
      resp.set_content(func::GetMedia("web/images/favicon.png"), "image/png");});

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
  std::cout << "ServerFrame: DoLogin" << std::endl;
  nlohmann::json input = ValidateJson(req, resp, {"username", "password"});
  if (input.empty()) {
    std::cout << "ServerFrame: DoLogin: failed (empty request or missing fields)!" << std::endl;
    return;
  }

  //Call DoLogin, returns "&msg: ..." in case of failure
  std::unique_lock ul(shared_mtx_user_manager_);
  std::string error = user_manager_.DoLogin(input["username"], input["password"]);
  ul.unlock();
  
  if (error != "") {
    std::cout << "ServerFrame: DoLogin: failed (Incorrect login data)!" << std::endl;
    resp.status = 401;
  }
  else {
    ul.lock();
    std::string cookie = "SESSID=" + user_manager_.GenerateCookie(input["username"])
      + "; Path=/";
    ul.unlock();
    resp.set_header("Set-Cookie", cookie.c_str());
    std::cout << "ServerFrame: DoLogin: success: " << cookie.c_str() << std::endl;
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
void ServerFrame::ServeFile(const Request& req, Response& resp, bool backup) const {
  std::cout << "ServeFile" << std::endl;
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") {
    std::cout << "ServeFile: Login Failed." << std::endl;
    return;
  }

  std::shared_lock sl(shared_mtx_user_manager_);
  User* user = user_manager_.GetUser(username);
  sl.unlock();

  if (user->CheckAccessToLocations(req.matches[0]) == false && req.matches.size() > 1) {
    resp.status = 302;
    resp.set_header("Location", "/overview");
    std::cout << "No access!" << std::endl;
  }
  else {
    std::string page = "";
    try {
      if (req.matches.size() == 1) {
        sl.lock();
        nlohmann::json shared_worlds = user_manager_.GetSharedWorlds(username);
        nlohmann::json all_worlds = user_manager_.GetAllWorlds(username);
        page = user->GetOverview(shared_worlds, all_worlds, textad_port_);
        std::cout << "ServeFile: sending overview..." << std::endl;
      }
      else {
        sl.lock();
        page = user_manager_.worlds()->GetPage(req.matches[0]);
        std::cout << "ServeFile: sending world-page..." << std::endl;
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
    else if (info_type == "notes") {
      std::cout << "calling Graph" << std::endl;
      object_json = user_manager_.worlds()->GetNotes(path);
    }

  }
  
  //Check whether action succeeded
  std::cout << "ServerFrame::GetObject" << info_type << " done with: " << object_json << ".\n\n";
  resp.set_content(object_json.dump(), "application/json");
}

void ServerFrame::SetNotes(const Request& req, Response& resp) {
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") {
    resp.status = 404;
    return;
  }

  //Try to parse json
  nlohmann::json input = ValidateJson(req, resp, {"notes", "path"});
  std::string notes = input["notes"];
  std::string path = input.value("path", "");

  std::cout << "SetNotes(" << path << ")\n" << notes << std::endl;

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
    std::cout << "calling Graph" << std::endl;
    user_manager_.worlds()->SetNotes(path, notes);
    resp.status = 200;
  }
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
  else if (req.matches.size() > 1 && req.matches[1] == "world") {
    std::string world_id = func::ConvertToId(name);
    infos["name"] = name;
    error_code = user_manager_.worlds()->CreateNewWorld("/"+username+"/files/"+world_id, 
        world_id, infos);
  }
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
  nlohmann::json json_req = ValidateJson(req, resp, {"json", "path"});
  std::string path = json_req["path"];
  bool force = json_req.value("force", false);
  if (json_req.empty()) 
    return;
  std::string id = "";
  // Use id 
  if (json_req["json"].contains("id")) {
    id = json_req["json"]["id"];
  }
  // Use last element of path as id
  // If values is array (for config-objects), us first key, and reduce json to value.
  else if (json_req["json"].size() > 1) {
    id = path.substr(path.rfind("/")+1);
	}
  else {
    for (const auto& it : json_req["json"].items()) {
      json_req["json"] = it.value();
      id = path.substr(path.rfind("/")+1);
      break;
    }
  }

  nlohmann::json modified_obj = json_req["json"];
  
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

void ServerFrame::WriteMedia(const Request& req, Response& resp, std::string media_type) {
  //Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;
  
  //Get user
  std::shared_lock sl(shared_mtx_user_manager_);
  User* user = user_manager_.GetUser(username);
  sl.unlock();

  if (req.files.size() != 2) {
    resp.status = 400;
    return;
  }

  const auto& file = req.get_file_value("photo");
  const auto& data = req.get_file_value("data");
  std::string json_str = data.content;
  nlohmann::json req_data = nlohmann::json::parse(json_str);

  if (!req_data.contains("name") || !req_data.contains("path")) { 
    std::cout << "Wrong json format." << std::endl;
    resp.status = 400;
    return;
  }

  if (!user->CheckAccessToLocations(req_data["path"])) {
    std::cout << "No acces writes." << std::endl;
    resp.status = 400;
    return;
  }

  std::filesystem::path path_to_media_loc = base_path_ 
    / req_data["path"].get<std::string>().substr(1) // remove leading slash ("/")
		/ req_data["name"].get<std::string>();
  path_to_media_loc.replace_extension(((media_type == "image") ? ".jpg" : ".mp3"));
  func::StoreMedia(path_to_media_loc, file.content);
  user_manager_.worlds()->GetWorldFromUrl(req_data["path"])->LoadWorld();

  resp.status = 200;
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

void ServerFrame::GetEditTemplate(const Request& req, Response& resp, std::string edit_template) const {
  nlohmann::json input = ValidateJson(req, resp, {"__creator", "__world"});
	std::cout << "ServerFrame::GetEditTemplate. Got json: " << input.dump() << std::endl;
  // Get availibe_fields (handlers, commands) and add to content json
  nlohmann::json availibe_fields;
  func::LoadJsonFromDisc("handlers.json", availibe_fields);
  input["__availibe_handlers"] = availibe_fields;
  func::LoadJsonFromDisc("commands.json", availibe_fields);
  input["__availibe_commands"] = availibe_fields;
  func::LoadJsonFromDisc("handler_arguments.json", availibe_fields);
  input["__handler_arguments"] = availibe_fields.dump();
  World* world = user_manager_.worlds()->GetWorld(input["__creator"], input["__world"]);
	// If world was found, add world-specific data 
	if (world) {
		availibe_fields = world->GetAttributes();
		input["_attributes"] = availibe_fields;
		availibe_fields = world->GetAttributeCategories();
		input["__attribute_categories"] = availibe_fields;
		availibe_fields = world->GetModTypes();
		input["_mod_types"] = availibe_fields;
	}


  std::cout << "ServerFrame::GetEditTemplate: " << input << std::endl;
  inja::Environment env;
  inja::Template events = env.parse_template("web/element_templates/events.html");
  env.include_template("web/element_templates/events", events);

  std::string path = "web/edit_templates/" + edit_template + ".html";
  std::cout << "ServerFrame::GetEditTemplate: laoding template at: " << path << std::endl;
  try {
    inja::Template temp = env.parse_template(path);
    env.add_void_callback("log", 1, [](inja::Arguments args) {
      std::cout << "logging: " << args.at(0)->get<std::string>() << std::endl;
    });
    std::string page_content = env.render(temp, input);
    resp.set_content(page_content, "text/txt");
		resp.status = 200;
  }
  catch(std::exception& e) {
    resp.set_content("Error parsing this page. We're sorry :(", "text/txt");
    std::cout << "ServerFrame::GetEditTemplate: an error ocured: " << e.what() << std::endl;
		resp.status = 500;
  }
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
  std::string type = req.matches[1];
  std::filesystem::path path = base_path_ / user / "logs" / (world + "_" + type);

  // Check if path exists.
  if (func::demo_exists(path) == false)
    resp.status = 401;
 
  // Convert to html
	std::cout << "Converting log to html: " << path << ".txt" << std::endl;
  std::string command = "cat " + path.string() + ".txt | aha > " + path.string() + ".html";
  system(command.c_str());
  resp.set_content(func::GetPage(path.string() + ".html"), "text/txt");
  resp.status = 200;
}

void ServerFrame::StartGame(const Request& req, Response& resp) {
  // Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  std::cout << "StartGame: " << req.body << std::endl;
  if (username == "") return;
  auto data = ValidateJson(req, resp, {"creator", "world_name"});
  if (data.size() == 0)
    return;
  
  // Extract data from path and build path to textadventure.
  std::string creator = data["creator"];
  std::string world_name = data["world_name"];
  std::filesystem::path path_to_game = base_path_ / creator / "files" / world_name;

  std::cout << "Got: " << creator << ", " << " world: " << world_name << ", Path: " << path_to_game << std::endl;
    
  // Get world (keep mutex locked to assure, that pointer is not changed. 
  std::cout << "StartGame: Getting world." << std::endl;
  std::shared_lock sl(shared_mtx_user_manager_);
  World* world = user_manager_.worlds()->GetWorld(creator, world_name);
  std::cout << "worlds exists: " << (world != nullptr) << std::endl;
  std::cout << "path exists: " << func::demo_exists(path_to_game) << std::endl;
  if (world == nullptr || !func::demo_exists(path_to_game)) {
    resp.status = 401;
    return;
  }
  std::cout << "StartGame: creating world." << std::endl;
  httplib::Client cl("localhost", textad_port_);
  cl.set_connection_timeout(2);
  nlohmann::json request = {{"creator", creator}, {"name", world_name}, 
    {"path", func::RemoveSequenzes(path_to_game, 1)}}; // move up one dir
  auto response = cl.Post("/api/create/", {}, request.dump(), "application/x-www-form-urlencoded");

  if (response) 
    resp.status = response->status;
  else 
    resp.status = 500;
}

void ServerFrame::CloseGame(const Request& req, Response& resp) {
  // Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;
  auto data = ValidateJson(req, resp, {"creator", "world_name"});
  if (data.size() == 0) {
    return;
  }
  
  // Extract data from path and build path to textadventure.
  std::string creator = data["creator"];
  std::string world_name = data["world_name"];
  httplib::Client cl("localhost", textad_port_);
  cl.set_connection_timeout(2);
  auto response = cl.Get("/api/close/" + creator + "/" + world_name);
  resp.status = response->status;
}


void ServerFrame::IsGameRunning(const Request& req, Response& resp) {
  // Try to get username from cookie
  std::string username = CheckLogin(req, resp);
  if (username == "") return;
  auto data = ValidateJson(req, resp, {"creator", "world_name"});
  if (data.size() == 0)
    return;
  
  // Extract data from path and build path to textadventure.
  std::string creator = data["creator"];
  std::string world_name = data["world_name"];
  httplib::Client cl("localhost", textad_port_);
  cl.set_connection_timeout(2);
  auto response = cl.Get("/api/running/" + creator + "/" + world_name);
  resp.status = response->status;
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
    resp.status = 400;
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
