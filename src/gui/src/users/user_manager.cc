/**
 * @author fux
*/

#include "user_manager.h"
#include "util/func.h"
#include "util/error_codes.h"
#include "world/worlds.h"

#include <exception>
#include <iostream>

namespace fs = std::filesystem;

UserManager::UserManager(std::string main_path) : path_(main_path) {
  ports_ = 9001;

  worlds_ = new Worlds(path_, 9001);

  //Iterate over users and create user.
  for (auto& p : fs::directory_iterator(path_)) {
    std::string path = p.path();
    std::ifstream read(path + "/user.json");
    nlohmann::json user;
    read >> user;
    
    try {
      users_[user["username"]] = new User(user["username"], user["password"], 
        path_, user["locations"]);
      InitWorlds(users_[user["username"]]);
    } catch(std::exception& e) {
      std::cout << "Creating user at path: " << p.path() << " failed!\n";
    }
  }
  std::cout << users_.size() << " users initialized!" << std::endl;
}

Worlds* UserManager::worlds() const {
  return worlds_;
}


User* UserManager::GetUser(std::string username) const {
  std::shared_lock sl(shared_mutex_users_);
  if (users_.count(username) > 0)
    return users_.at(username);
  return nullptr;
}

void UserManager::AddUser(std::string username, std::string password) {
  std::scoped_lock scoped_locks(shared_mutex_users_, shared_mutex_ports_);
  users_[username] = new User(username, password, path_, categories_);
  ports_+=2;
  users_[username]->SafeUser();
}

void UserManager::DeleteUser(std::string username) {
  std::unique_lock ul_users(shared_mutex_users_);
  //TODO (fux): how do I handle thread safty here?
  delete users_[username];
  users_.erase(username);
  ul_users.unlock();
  std::unique_lock ul_cookies(shared_mutex_cookies_);
  for (auto it=cookies_.begin(); it!=cookies_.end();) {
    if (it->second == username) 
      cookies_.erase(it++);
    else
      ++it;
  }
  ul_cookies.unlock();

  try {
    fs::remove_all(path_ + "/" + username);
    std::cout << "User " << username << " deleted." << std::endl;
  }
  catch (std::exception& e) {
    std::cout << "Deleting user " << username << " failed: " << e.what() 
      << std::endl;
  }
}

bool UserManager::DoLogout(const char* ptr) {
  if (!ptr) return false;

  //Try to get user
  std::string username = GetUserFromCookie(ptr);

  //Parse user-cookie-id from cookie and check if this user exists.
  std::string cookie = ptr;
  cookie = cookie.substr(cookie.find("SESSID=")+7); 
  std::unique_lock ul(shared_mutex_cookies_);
  if (cookies_.count(cookie) == 0) 
    return false;
  cookies_.erase(cookie);
  return true;
}

std::string UserManager::DoLogin(std::string username, std::string password) {
  //Check if logging succeeds 
  nlohmann::json login_success = CheckLogin(username, password);
  if (login_success.count("error") > 0) 
    return login_success["error"];

  std::shared_lock sl(shared_mutex_users_);
  users_.at(username)->SafeUser();
  return ""; 
}

nlohmann::json UserManager::CheckLogin(std::string username, std::string password) const {
  //hash password
  password = func::hash_sha3_512(password);
  std::shared_lock sl(shared_mutex_users_);
  //check whether user exists
  if (users_.count(username) == 0)
    return nlohmann::json{{"error", "Username does not exist."}}; 

  //Check if password matches
  if (users_.at(username)->password() != password) 
    return nlohmann::json{{"error", "Incorrect password."}}; 

  return nlohmann::json();
}

std::string UserManager::DoRegistration(std::string username, std::string pw1, 
    std::string pw2) {
  if (!CheckPasswordStrength(pw1)) 
    return "Strength insufficient!";
    
  //Check whether passwords match
  if (pw1 != pw2) 
    return "Passwords do not match!";

  //Check whether user exists
  std::shared_lock sl(shared_mutex_users_);
  if (users_.count(username) > 0)
    return "Username already exists!";
  sl.unlock();

  //If registration is successfull, create new user.
  AddUser(username, func::hash_sha3_512(pw1)); 
  return "";
}

bool UserManager::CheckPasswordStrength(std::string password) const {
  if (password.length() >= 15) return true;
  if (password.length() < 8) return false;
  bool digit = false, letter = false;
  for (size_t i=0; i<password.length(); i++) {
    if (std::isdigit(password[i]) != 0) 
      digit = true;
    else if (std::islower(password[i]) != 0) 
      letter = true;
    if (letter == true && digit == true) 
      return true;
  }
  return false;
}

int UserManager::GrantAccessTo(std::string user1, std::string user2, std::string world) {
  //Check is both given users exist
  if (users_.count(user1) == 0 || users_.count(user2) == 0)
    return ErrorCodes::NO_USER;

  //Check if user who tries to grant acces has access himself.

  //Check if world exists.
  if (!func::demo_exists(path_+"/"+user1+"/files/"+world))
    return ErrorCodes::NO_WORLD;

  try {
    users_[user2]->AddLocation(user1, world);
    users_[user1]->RemoveRequest(user2, world);
  }
  catch(std::exception& e) {
    std::cout << "GrantAccessTo failed:" << e.what() << std::endl;
    return ErrorCodes::FAILED;
  }
  return ErrorCodes::SUCCESS;
}

std::string UserManager::GenerateCookie(std::string username) {
  //Collect 32 random bytes in Linux provided by /dev/urandom
	std::ifstream ifs("/dev/urandom", std::ios::in|std::ios::binary);
	if (!ifs)
		std::cout << "Could not generate session id!" << std::endl;
	char buffer[33];
	ifs.read(buffer,32);
	ifs.close();
  
	//Assign each character in the buffer a random char based on the byte
	//read from the random device
  static const char alphanum[] =
		"0123456789""ABCDEFGHIJKLMNOPQRSTUVWXYZ""abcdefghijklmnopqrstuvwxyz""_";
  std::string sessid = "";
	for (int i = 0; i< 32; ++i)
		sessid += alphanum[buffer[i] % (sizeof(alphanum)-1)];
   
  std::unique_lock ul(shared_mutex_cookies_);
  cookies_[sessid] = username; 
  return sessid;
}


std::string UserManager::GetUserFromCookie(const char* ptr) const {
  if (!ptr) 
    return "";

  //Parse user-cookie-id from cookie and check if this user exists.
  std::string cookie = ptr;
  cookie = cookie.substr(cookie.find("SESSID=")+7); 
  std::shared_lock sl(shared_mutex_cookies_);
  if (cookies_.count(cookie) == 0) 
    return "";
  return cookies_.at(cookie);
}

void UserManager::InitWorlds(User* user) {
  for (auto& p : fs::directory_iterator(path_ + "/" 
        + user->username() + "/files/")) {
    user->worlds()[p.path().stem()] = ports_;
    ports_+=2;
  }
}

int UserManager::GetNextPort() {
  ports_+=2;
  return ports_;
}

nlohmann::json UserManager::GetAllWorlds(std::string username) const {
  nlohmann::json all_worlds = nlohmann::json::array();
  for (auto it : users_) {
    if (it.first == username) continue;
    for (auto jt : it.second->worlds())
      all_worlds.push_back({{"user", it.first},{"name", jt.first},{"port", jt.second}});
  }
  return all_worlds;
}

nlohmann::json UserManager::GetSharedWorlds(std::string username) const {
  nlohmann::json shared_worlds = nlohmann::json::array();
  for(auto it : GetUser(username)->locations()) {
    std::string location = it;
    if (location.find(username) == 0) continue;
    if (location.find("backups") != std::string::npos) continue;
    std::string user = location.substr(0, location.find("/"));
    std::string world = location.substr(location.rfind("/")+1);
    if (GetUser(user) != nullptr) {
      shared_worlds.push_back(nlohmann::json({{"user", user}, {"name", world}, 
          {"port", GetUser(user)->worlds()["world"]}}));
    }
  }
  return shared_worlds;
}

int UserManager::GetPortOfWorld(std::string user, std::string world) const {
  try {
    return users_.at(user)->worlds()[world];
  }
  catch(std::exception& e) {
    std::cout << "World \"" << world << "\" from " << user << " not found.\n";
    return 0;
  }
}
