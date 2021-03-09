/**
 * @author fux
*/
#include "user.h"
#include "util//error_codes.h"
#include "util/func.h"

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>

namespace fs = std::filesystem;

User::User(std::string name, std::string pw, std::string path) : username_(name), path_(path) {
  password_ = pw;
  locations_.push_back(username_ + "/");

  try {
    fs::create_directory(path_+"/"+username_);
    fs::create_directory(path_+"/"+username_+"/files");
    fs::create_directory(path_+"/"+username_+"/backups");
    fs::create_directory(path_+"/"+username_+"/logs");
  }
  catch (std::exception& e) {
    std::cout << "Creating essential files failed: " << e.what() << std::endl;
  }
}

User::User(std::string name, std::string pw, std::string path, 
    std::vector<std::string> locations) : username_(name), path_(path) {
  password_ = pw;
  locations_ = locations;

  try {
    fs::create_directory(path_+"/"+username_);
    fs::create_directory(path_+"/"+username_+"/files");
    fs::create_directory(path_+"/"+username_+"/backups");
    fs::create_directory(path_+"/"+username_+"/logs");
  }
  catch (std::exception& e) {
    std::cout << "Creating essential files failed: " << e.what() << std::endl;
  }
}

//** Getter ** //

std::string User::password() const {
  std::shared_lock sl(shared_mtx_password_);
  return password_;
}
std::map<std::string, int>& User::worlds() {
  return worlds_;
}
const std::string User::username() const {
  return username_;
}
const std::vector<std::string>& User::locations() const {
  return locations_;
}


// ** Setter ** //

void User::set_password(std::string password) {
  std::unique_lock ul(shared_mtx_password_);
  password_ = password;
}

void User::AddLocation(std::string user, std::string world) {
  locations_.push_back(user+"/files/"+world);
  locations_.push_back(user+"/backups/"+world);
}

// ** Serve and generate pages ** //

std::string User::GetOverview(nlohmann::json shared_worlds, 
    nlohmann::json all_worlds) {
  //Create initial json with username.
  nlohmann::json worlds = nlohmann::json({{"username", username_}});

  //Add all dictionaries of this user to json.
  worlds["worlds"] = nlohmann::json::array();
  for (auto& p : fs::directory_iterator(path_ + "/" + username_ + "/files")) {
    if (p.path().stem() != "user") {
      worlds["worlds"].push_back(nlohmann::json({{"name",p.path().stem()},
          {"port",worlds_[p.path().stem()]}}));
    }
  }
  worlds["shared_worlds"] = shared_worlds;
  worlds["all_worlds"] = all_worlds;
  worlds["requests"] = requests_;

  //Parse overview page. 
  inja::Environment env;                                                                  
  inja::Template temp = env.parse_template("web/overview_template.html");
  return env.render(temp, worlds);
}

std::string User::GetBackups(std::string user, std::string world) {
  //Get all backups for this world.
  std::vector<std::string> backups;
  for (auto& p  : fs::directory_iterator(path_+"/"+user+"/backups")) {
    std::string backup = p.path().filename();
    if (backup.find(world) == 0) {
      backups.push_back(backup);
    }
  }

  //Parse backup page. 
  nlohmann::json j = nlohmann::json({{"user", username_}, {"world", world}, 
      {"backups", backups}});
  inja::Environment env;                                                                  
  inja::Template temp = env.parse_template("web/backups_template.html");
  return env.render(temp, j);
}

// ** Functions ** //

int User::AddFile(std::string path, std::string name) {
  path = path_ + path;
  //Check if path exists
  if (!func::demo_exists(path)) 
    return ErrorCodes::PATH_NOT_FOUND;
  //Check for wrong format
  if (name.find("/") != std::string::npos)
    return ErrorCodes::WRONG_FORMAT;
  //Replace space by underscore and check if file already exists.
  std::replace(name.begin(), name.end(), ' ', '_');
  if (func::demo_exists(path + "/" + name + ".json"))
    return ErrorCodes::ALREADY_EXISTS;

  //Get Category from string
  std::string category = path.substr(path.rfind("/") + 1);

  //Check for not supported categories
  if (category == "players")
    return ErrorCodes::NOT_ALLOWED;

  nlohmann::json file;
  if (category == "defaultDialogs")
    file = nlohmann::json::array();
  else 
    file = nlohmann::json::object();
  
  try {
    std::ofstream write(path + "/" + name + ".json");
    write << file;
    write.close();
  }
  catch (std::exception& e) {
    std::cout << "Adding new file failed!: " << e.what() << std::endl;
    return ErrorCodes::FAILED;
  }
  return ErrorCodes::SUCCESS;
}

int User::AddNewObject(std::string path, std::string id, bool force) {
  std::string full_path = path_ + path + ".json";
  //Check if path exists
  if (!func::demo_exists(full_path)) 
    return ErrorCodes::PATH_NOT_FOUND;
  //Check for wrong format
  if (id.find("/") != std::string::npos)
    return ErrorCodes::NOT_ALLOWED;
  //Replace space by underscore and check if file already exists.
  std::replace(id.begin(), id.end(), ' ', '_');
  if (func::demo_exists(full_path + "/" + id + ".json"))
    return ErrorCodes::ALREADY_EXISTS;

  //Get Category
  std::vector<std::string> elements = func::Split(path, "/");
  if (elements.size() < 2)
    return ErrorCodes::WRONG_FORMAT;
  std::string category = elements[elements.size()-2];
  
  //Load empty object of category
  nlohmann::json request;
  request["force"] = force;
  request["path"] = path+"/"+id;
  if (category == "players") {
    nlohmann::json config;
    if (!func::LoadJsonFromDisc("../../data/default_jsons/config.json", config))
      return ErrorCodes::PATH_NOT_FOUND;
    nlohmann::json player;
    player = config["new_player"];
    std::cout << "NEW PLAYERS: " << player << std::endl;
    player["id"] = id;
    request["json"] = player;
    std::cout << "SENDING PLAYER JSON: " << request["json"] << std::endl;
  }
  else if (category == "defaultDialogs") 
    request["json"] = nlohmann::json::array();
  else if (category == "defaultDescriptions")
    request["json"] = nlohmann::json::array();
  else 
    request["json"] = nlohmann::json{{"id", id}};

  return WriteObject(request.dump());
}

int User::WriteObject(std::string request) {
  //Get values from request
  nlohmann::json json;
  std::string path;
  bool force = false, direct = false;
  try {
    nlohmann::json j = nlohmann::json::parse(request);
    json = j["json"];
    std::cout << "GOT PLAYER JSON: " << json << std::endl;
    path = j["path"];

    force = j.value("force", false);
    direct = j.value("direct", false);
  }
  catch (std::exception& e) {
    std::cout << "WriteObject: Problem parsing request: " << e.what() << "\n";
    return ErrorCodes::WRONG_FORMAT;
  }

  //Check is user has access
  if (CheckAccessToLocations(path) == false)
    return ErrorCodes::ACCESS_DENIED;

  //Parse path, read json and create doublicate (backup)
  std::string path_to_object = path_;
  if (direct == true)
    path_to_object += path + ".json";
  else
    path_to_object += path.substr(0, path.rfind("/")) + ".json";

  nlohmann::json object;
  if (!func::LoadJsonFromDisc(path_to_object, object))
    return ErrorCodes::PATH_NOT_FOUND;
  nlohmann::json obj_backup = object;
  
  //Modify and write object.
  if (direct == true) 
    object = json;
  else if (json.is_array() == true && object.is_array() == true)
    object.push_back(json);
  else if (json.is_array() == true)
    object[std::to_string(object.size())] = json;
  else  {
    std::cout << "Correctly chose object-type. Writing json: " << json << std::endl;
    object[json["id"].get<std::string>()] = json;
  }

  std::cout << "WRITING" << object << std::endl;

  std::ofstream write(path_to_object);
  write << object;
  write.close();

  //Check if game is still running and return true if yes, are forcing is true
  if (CheckGameRunning(path) == true || force == true)
    return ErrorCodes::SUCCESS;

  //If not and forcing to write is set to false
  std::ofstream write_backup(path_to_object);
  write_backup << obj_backup;
  write_backup.close();
  return ErrorCodes::GAME_NOT_RUNNING;
}

int User::DeleteFile(std::string path, std::string file) {
  //Load json 
  path = path_+path+"/"+file+".json";

  if (func::demo_exists(path) == false)
    return ErrorCodes::PATH_NOT_FOUND;

  //Write remove file 
  try {
    fs::remove_all(path);
  }
  catch (std::exception& e) {
    std::cout << "DeleteFile failed: " << e.what() << std::endl;
    return ErrorCodes::FAILED;
  }
  return ErrorCodes::SUCCESS;
}

int User::DeleteObject(std::string path, std::string object) {
  //Load json 
  path = path_+path+".json";
  nlohmann::json json_objects;
  if (func::LoadJsonFromDisc(path, json_objects) == false)
    return ErrorCodes::PATH_NOT_FOUND;

  //Delete object
  if (json_objects.count(object) == 0)
    return ErrorCodes::NOT_ALLOWED;
  json_objects.erase(object);

  //Write back to disc
  try {
    std::ofstream write_backup(path);
    write_backup << json_objects;
    write_backup.close();
  }
  catch (std::exception& e) {
    std::cout << "DeleteObject failed: " << e.what() << std::endl;
    return ErrorCodes::FAILED;
  }
  return ErrorCodes::SUCCESS;
}



void User::SafeUser() const {
  try { 
    fs::create_directory(path_ + "/" + username_);
  }
  catch(...) {}
  std::string path = path_ + "/" + username_ + "/user.json";  
  std::ofstream write(path);
  if (!write) {
    std::cout << "Serious error: json not found: " << path << std::endl;
    return;
  }

  nlohmann::json user = ConstructJson();
  write << user;
  write.close();
}

nlohmann::json User::ConstructJson() const {
  std::shared_lock sl(shared_mtx_password_);
  nlohmann::json user;
  user["username"] = username_;
  user["password"] = password_;
  user["locations"] = locations_;
  return user;
}



int User::CreateBackup(std::string user, std::string world) {
  std::string path = path_ + "/" + user + "/files/" + world;
  
  if (!func::demo_exists(path)) return ErrorCodes::PATH_NOT_FOUND;

  //Create backup name ([word]_[YYYY-mm-dd_HH-MM-ss])
  time_t now;
  std::time(&now);
  char buf_human[sizeof("YYYY-mm-dd_HH-MM-ss")];
  strftime(buf_human, sizeof(buf_human), "%F_%T", localtime(&now));
  std::string path_backup = path_+"/"+user+"/backups/"+world+"_"+buf_human;
  std::replace(path_backup.begin(), path_backup.end(), ':', '-');

  //Create backup, by copying all files to selected directory
  try { 
    fs::copy(path, path_backup, fs::copy_options::recursive); 
  }
  catch (std::exception& e) { 
    std::cout << "Copying failed: " << e.what() << std::endl; 
    return ErrorCodes::FAILED;
  }
  return ErrorCodes::SUCCESS;
}


int User::RestoreBackup(std::string user, std::string backup) {
  if (user != username_) return ErrorCodes::ACCESS_DENIED;

  //Extract world from backup
  std::string world = backup.substr(0, backup.rfind("_"));
  world = world.substr(0, world.rfind("_"));

  //Create path to selected backup and world.
  std::string path = path_+"/"+user+"/backups/"+backup;
  std::string path_to_world = path_+"/"+user+"/files/"+world;

  //Check if both paths exist.
  if (!func::demo_exists(path) || !func::demo_exists(path_to_world)) {
    std::cout << "Backup " << path << " doesn't exist! Or ";
    std::cout << "World " << path_to_world << " doesn't exist!" << std::endl;
    return ErrorCodes::PATH_NOT_FOUND;
  }

  //Restore backup.
  const auto copy_options = fs::copy_options::update_existing
                          | fs::copy_options::recursive;
  try {
    fs::remove_all(path_to_world);  //Delete original
    fs::copy(path, path_to_world, copy_options);  //Copy files to original location
  }
  catch (std::exception& e) { 
    std::cout << "Copying failed: " << e.what() << std::endl; 
    return ErrorCodes::FAILED;
  }
  return ErrorCodes::SUCCESS;
}

int User::DeleteBackup(std::string user, std::string backup) {
  if (user != username_) return ErrorCodes::ACCESS_DENIED;

  //Build path, then check if path exists.
  std::string path = path_+"/"+user+"/backups/"+backup;
  if (!func::demo_exists(path)) {
    std::cout << "Backup " << path << " does not exist!" << std::endl;
    return ErrorCodes::PATH_NOT_FOUND;
  }

  //Remove backup.
  fs::remove_all(path);
  return ErrorCodes::SUCCESS;
}

int User::GetPortOfWorld(std::string world) {
  if (worlds_.count(world) == 0) {
    std::cout << "World not found" << std::endl;
    return -1;
  }
  return worlds_[world];
}

bool User::AddRequest(std::string user, std::string world) {
  std::string path = path_ + "/" + username_ + "/files/" + world;
  if (!func::demo_exists(path)) {
    std::cout << "World at path: " << path << " doesn't exist." << std::endl;
    return false;
  }

  requests_.push_back(nlohmann::json({{"user1", username_}, {"user2", user},
        {"world", world}}));
  return true;
}

void User::RemoveRequest(std::string user, std::string world) {
  size_t counter=0;
  for (auto it : requests_) {
    if (it["user2"] == user && it["world"] == world) {
      requests_.erase(requests_.begin()+counter);
      return;
    }
    counter++;
  }
  std::cout << "request not found." << std::endl;
}

bool User::CheckAccessToLocations(std::string path) { 
  std::cout << "CheckAccessToLocations: " << path << std::endl; 
  for (const auto& it : locations_) {
    std::cout << "Location: " << it << std::endl;
    if (path.find("/"+it) == 0) {
      return true;
    }
  }
  return false; 
}

bool User::CheckGameRunning(std::string path) {

  //Try to extract information (user and world, complete path)
  std::vector<std::string> vec = func::Split(path, "/");
  if (vec.size() < 3) return false;
  std::string user = vec[0];
  std::string world = vec[2];
  std::string path_to_txtad = path_+path;

  //Create command
  std::string command = "./../../textadventure/build/bin/testing.o "
    "--path ../../data/users/"+user+"/files/"+world+"/";

  //Get players from players.json
  nlohmann::json players;
  std::string path_to_players = path_+"/"+user+"/files/"+world
    +"/players/players.json";
  if (func::LoadJsonFromDisc(path_to_players, players) == false) {
    std::cout << "Failed loading players json" << std::endl;
    return false;
  }
  
  //Run game with every existing player.
  bool success = true;
  for (auto it=players.begin(); it!=players.end(); it++) {
    std::cout << "PLAYER: " << it.value() << std::endl;
    std::string test_p = command + " -p " + it.key() +
      " > ../../data/users/"+user+"/logs/"+world+"_write.txt";
    if (system(test_p.c_str()) != 0) 
      success = false;
    std::cout << "Running with player \"" << it.key() << "\": " << success << std::endl;
  }
  return success;
}
