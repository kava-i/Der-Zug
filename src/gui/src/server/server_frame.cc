#include "server_frame.h"
#include "util/func.h"

using namespace httplib;


//Constructor
ServerFrame::ServerFrame() : user_manager_("../../data/users/", {"attacks", 
    "defaultDialogs", "dialogs", "players", "rooms", "characters", 
    "defaultDescriptions", "details", "items", "quests", "texts"})
{}

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

void ServerFrame::Overview(const Request& req, Response& resp) const {
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
    std::string page = user_manager_.GetUser(username)->GetOverview();
    sl.unlock();
    resp.set_content(page.c_str(), "text/html");
  }
}

void ServerFrame::World(const Request& req, Response& resp) const {
  std::cout << "World" << std::endl;
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
    std::string page = user_manager_.GetUser(username)->GetWorld(req.matches[1]);
    sl.unlock();
    resp.set_content(page.c_str(), "text/html");
  }
}

void ServerFrame::Category(const Request& req, Response& resp) const {
  std::cout << "Category" << std::endl;
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
    std::string page = user_manager_.GetUser(username)->GetCategory(
        req.matches[1], req.matches[2]);
    sl.unlock();
    resp.set_content(page.c_str(), "text/html");
  }
}

void ServerFrame::SubCategory(const Request& req, Response& resp) const {
  std::cout << "SubCategory" << std::endl;
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
    std::string page = user_manager_.GetUser(username)->GetObjects(
        req.matches[1], req.matches[2], req.matches[3]);
    sl.unlock();
    resp.set_content(page.c_str(), "text/html");
  }
}

void ServerFrame::Object(const Request& req, Response& resp) const {
  std::cout << "SubCategory" << std::endl;
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
    std::string page = user_manager_.GetUser(username)->GetObject(
        req.matches[1], req.matches[2], req.matches[3], req.matches[4]);
    sl.unlock();
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

