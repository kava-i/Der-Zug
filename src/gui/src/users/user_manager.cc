/**
 * @author fux
*/

#include "user_manager.h"
#include "util/func.h"
namespace fs = std::filesystem;

UserManager::UserManager(std::string main_path, std::vector<std::string> cats) 
  : path_(main_path), categories_(cats) {
  //Iterate over users and create user.
  for (auto& p : fs::directory_iterator(path_)) {
    std::string path = p.path();
    std::ifstream read(path + "/user.json");
    nlohmann::json user;
    read >> user;
    
    try {
      users_[user["username"]] = new User(user["username"], user["password"], 
        path_, user["locations"], categories_);
    }
    catch(std::exception& e) {
      std::cout << "Creating user at path: " << p.path() << " failed!\n";
    }
  }
  std::cout << users_.size() << " users initialized!" << std::endl;
}


/**
 * Returns given user, if exists
 * @param[in] username
 * @return user, if exists, nullptr otherwise
 */
User* UserManager::GetUser(std::string username) const {
  std::shared_lock sl(shared_mutex_users_);
  if (users_.count(username) > 0)
    return users_.at(username);
  return nullptr;
}

/**
* Adds new user.
* @param[in] username
* @param[in] password
*/
void UserManager::AddUser(std::string username, std::string password) {
  std::unique_lock ul(shared_mutex_users_);
  users_[username] = new User(username, password, path_, categories_);
  users_[username]->SafeUser();
}

/**
 * Detele user.
 * Deletes object, erases from map and delete users jsons file.
 * @param[in] username
 */
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

  fs::remove(path_ + username + ".json");
  std::cout << "User " << username << " deleted." << std::endl;
}

 /**
 * Logout user. 
 * try to get user from cookie. Logout user, reset controller update 
 * interval and erase cookie. If user does not exist, try only to delete 
 * cookie from map.
 * @param[in] cookie.
 * @return false if cookie does not exist in map.
 */
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

/**
 * Check whether login was successful. Return error if not. 
 * @param username
 * @param password
 * @return error or empty string
 */
std::string UserManager::DoLogin(std::string username, std::string password) {
  //Check if logging succeeds 
  nlohmann::json login_success = CheckLogin(username, password);
  if (login_success.count("error") > 0) 
    return login_success["error"];

  std::shared_lock sl(shared_mutex_users_);
  users_.at(username)->SafeUser();
  return ""; 
}

/**
 * Check if username and password match/ exist. Return error-code if not.
 * Return empty json if login succeeded. 
 * @param[in] username
 * @param[in] password (Already hashed!!)
 * @return json with error code if no success.
 */
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

/**
 * Check whether registration was successful. Return error or empty string. 
 * @param username
 * @param pw1
 * @param pw2
 * @return error code or empty string.
 */
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

/**
 * @brief checking password strength
 * Either 15 characters long, or 8 characters + 1 lowercase + 1 digit.
 * @param password (given password to check)
 * @return whether strength is sufficient.
 */
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

/**
 * Create random 32 characters to generates cookie. And maps cookie and given
 * user.
 * @param[in] username (username which is mapped on cookie)
 * @return returns cookie as string.
 */
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


/**
 * Get user from cookie.
 * @param[in] resp (reference to response)
 * @return username, "$no_cookie" or "$no_user"
 */
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
