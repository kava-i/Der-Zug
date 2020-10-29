#include "server_frame.h"
#include "util/func.h"

using namespace httplib;


//Constructor
ServerFrame::ServerFrame() : user_manager_("../../data/users/", {"attacks", 
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
  server_.Post("/api/user_logout", [&](const Request& req, Response& resp) {
      DoLogout(req, resp); });
  server_.Post("/api/create_backup", [&](const Request& req, Response& resp) {
      Backups(req, resp, "create"); });
  server_.Post("/api/restore_backup", [&](const Request& req, Response& resp) {
      Backups(req, resp, "restore"); });
  server_.Post("/api/delete_backup", [&](const Request& req, Response& resp) {
      Backups(req, resp, "delete"); });
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
  else if (user->CheckAccessToLocations(req.matches[0]) == false) {
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
        page = user->GetObject(req.matches[2], req.matches[3], 
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
    bool success = user_manager_.GetUser(username)->WriteObject(req.body);
    sl.unlock();

    if (success == true)
      resp.status = 200;
    else
      resp.status = 401;
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
  }
  else {
    sl.lock();
    //Call matching function.
    bool success = false;
    if (action == "create")
      success = user_manager_.GetUser(username)->CreateBackup(req.body);
    else if (action == "restore") 
      success = user_manager_.GetUser(username)->RestoreBackup(req.body);
    else if (action == "delete")
      success = user_manager_.GetUser(username)->DeleteBackup(req.body);
    else
      success = false;
    sl.unlock();

    if (success == true)
      resp.status = 200;
    else
      resp.status = 401;
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
