/**
 * @author fux
*/
#include "user.h"
#include "util/func.h"

namespace fs = std::filesystem;

User::User(std::string name, std::string pw, std::string path, std::vector
    <std::string> cats) : username_(name), path_(path), categories_(cats) {
  password_ = pw;
  locations_.push_back(username_ + "/");
}

User::User(std::string name, std::string pw, std::string path, 
    std::vector<std::string> locations, std::vector
    <std::string> cats) : username_(name), path_(path), categories_(cats) {
  password_ = pw;
  locations_ = locations;
}
//** Getter ** //

std::string User::password() const {
  std::shared_lock sl(shared_mtx_password_);
  return password_;
}

// ** Setter ** //

void User::set_password(std::string password) {
  std::unique_lock ul(shared_mtx_password_);
  password_ = password;
}

// ** Serve an generate pages ** //

std::string User::GetOverview() {
  std::cout << "GetOverview" << std::endl;
  //Create initial json with username.
  nlohmann::json worlds = nlohmann::json({{"username", username_}});

  //Add all dictionaries of this user to json.
  for (auto& p : fs::directory_iterator(path_ + "/" + username_ + "/files")) {
    if (p.path().stem() != "user")
      worlds["worlds"].push_back(p.path().stem());
  }
  for (size_t i=1; i<locations_.size(); i++) {
    worlds["worlds"].push_back(locations_[i]);
  }
    
  //Parse overview page. 
  inja::Environment env;                                                                  
  inja::Template temp = env.parse_template("web/overview_template.html");
  return env.render(temp, worlds);
}

std::string User::GetWorld(std::string path, std::string world) {
  std::cout << "GetWorld" << std::endl;
  //Create initial json with world and all categories.
  nlohmann::json j = nlohmann::json({{"user", username_}, {"world", world}, 
      {"categories", categories_}});

  //Create dictionaries for all categories, if they do not exist.
  for (auto cat : categories_)
    fs::create_directory(path_+path+"/"+cat);

  //Parse world page. 
  inja::Environment env;                                                                  
  inja::Template temp = env.parse_template("web/in_world_template.html");
  return env.render(temp, j);
}

std::string User::GetBackups(std::string world) {
  std::cout << "GetBackups" << std::endl;
  //Get all backups for this world.
  std::vector<std::string> backups;
  for (auto& p  : fs::directory_iterator(path_ + "/backups")) {
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

std::string User::GetCategory(std::string world, std::string category) {
  std::cout << "GetCategory" << std::endl;
  //Build path to category and create initial json.
  std::string path = path_+"/files/"+world+"/"+category;
  nlohmann::json j_category = nlohmann::json({{"user", username_}, {"world", world}, 
      {"category", category}});

  inja::Environment env;
  inja::Template temp;

  //Directly parse config page.
  if (category == "config") {
    std::ifstream read(path+".json");
    nlohmann::json j;
    read >> j;
    read.close();
    j_category["json"] = j;
    temp = env.parse_template("web/in_json_template.html");
  }

  //Add all files in category to json and parse json.
  else {
    j_category["sub_categories"] = nlohmann::json::array();
    for (auto p : fs::directory_iterator(path+"/")) 
      j_category["sub_categories"].push_back(p.path().stem());
    temp = env.parse_template("web/in_category_template.html");
  }
  return env.render(temp, j_category);
}

std::string User::GetObjects(std::string world, std::string category, 
      std::string sub) {
  std::cout << "GetObjects" << std::endl;
  //Create path and trying to load into json 
  std::string path = path_+"/files/"+world+"/"+category+"/"+sub+".json";
  nlohmann::json objects;
  try{ 
    std::ifstream read(path);
    read>>objects;
    read.close();
  }
  catch(...) { 
    std::cout << "file: " << path << "not found.";
    return "";
  }

  inja::Environment env;
  nlohmann::json overview = nlohmann::json({{"user", username_}, 
      {"world", world}, {"category", category}, {"sub", sub}, 
      {"objects", nlohmann::json::array()}});
  
  //Standard style, for rooms, items, ... Dictionary or dictionaries
  if (objects.is_array() == false) {
    size_t couter = 1;
    for (auto obj : objects) {
      if (obj.is_array() == false)
        overview["objects"].push_back(obj["id"]);
      else
        overview["objects"].push_back(std::to_string(couter));
      couter++;
    }
  }
  
  //Style for defaultDialogs, array of arrays
  else {
    for (size_t i=1; i<=objects.size(); i++) 
      overview["objects"].push_back(sub + "_" + std::to_string(i));
  }
  inja::Template temp = env.parse_template("web/in_sub_category_template.html");
  return env.render(temp, overview);
}

std::string User::GetObject(std::string world, std::string category, 
        std::string sub, std::string obj) {
  std::cout << "GetObject" << std::endl;
  //Create path and trying to load into json 
  std::string path = path_+"/files/"+world+"/"+category+"/"+sub+".json";
  nlohmann::json objects;
  try{ 
    std::ifstream read(path);
    read>>objects;
    read.close();
  }
  catch(...) { 
    std::cout << "file: " << path << "not found.";
    return "";
  }

  nlohmann::json overview;   
  //Load object by id
  if (objects.is_array() == false) {
    if (objects.count(obj) > 0)
      overview = objects[obj];
  }
  //Load object by number
  else {
    unsigned int num = std::stoi(obj.substr(obj.length()-1))-1;
    if (objects.size() > num)
      overview["json"] = objects[num];
  }
  overview["header"] = nlohmann::json({{"user", username_}, {"world", world}, 
      {"category", category}, {"sub", sub}, {"obj",obj}});

  inja::Environment env;
  inja::Template temp;

  //Parse standard templates for descriptions and the header.
  inja::Template description_template = env.parse_template("web/description_template.html");
  env.include_template("web/temp_description", description_template);
  inja::Template room_description_template = 
    env.parse_template("web/room_description_template.html");
  env.include_template("web/temp_room_description", room_description_template);
  inja::Template object_header_template = env.parse_template("web/object_header_template.html");
  env.include_template("web/temp_header", object_header_template);

  //Parse different objects.
  if (category == "rooms")
    temp = env.parse_template("web/in_room_template.html");
  else if (category == "characters")
    temp = env.parse_template("web/in_char_template.html");
  else
    temp = env.parse_template("web/config_template.html");
  return env.render(temp, overview);
}

// ** Functions ** //


std::string User::CreateNewWorld(std::string name) {

  //Check for "/" which is considered bad format!
  if (name.find("/") != std::string::npos)
    return "Wrong format.";

  //Create path to new world and check whether world already exists.
  std::string path = path_ + "/" + username_ + "/files/" + name;
  if (func::demo_exists(path) == true)
    return "World already exists.";

  //Try creating world and all categories.
  try {
    //Create directory for world
    fs::create_directories(path);
    std::cout << "Created directory." << std::endl;

    //Create all subcategories.
    for (const auto& category : categories_) {
      std::cout << "Creating: " << path << "/" << category << std::endl;
      fs::create_directory(path + "/" + category);
    }
  }

  //Return error code and delete all already created directories.
  catch (std::exception& e) {
    std::cout << "Error creating new world or subdirectories: ";
    std::cout << e.what() << std::endl;
    fs::remove_all(path);
    return "Error creating world.";
  }
  return "";
}

bool User::CheckAccessToLocations(std::string path) { 
  std::cout << "CheckAccessToLocations: " << path << std::endl; 
  return true; 
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



bool User::CreateBackup(std::string world) {
  std::string path = path_ + "/files/" + world;
  
  if (!func::demo_exists(path)) return false;

  //Create backup name ([word]_[YYYY-mm-dd_HH-MM-ss])
  time_t now;
  std::time(&now);
  char buf_human[sizeof("YYYY-mm-dd_HH-MM-ss")];
  strftime(buf_human, sizeof(buf_human), "%F_%T", localtime(&now));
  std::string path_backup = path_ + "/backups/" + world + "_" + buf_human;
  std::replace(path_backup.begin(), path_backup.end(), ':', '-');

  //Create backup, by copying all files to selected directory
  try { 
    fs::copy(path, path_backup, fs::copy_options::recursive); 
  }
  catch (std::exception& e) { 
    std::cout << "Copying failed: " << e.what() << std::endl; 
    return false;
  }
  return true;
}

bool User::WriteObject(std::string request) {
  //Get values from request
  nlohmann::json json;
  std::string path;
  try {
    nlohmann::json j = nlohmann::json::parse(request);
    json = j["json"];
    path = j["path"];
  }
  catch (std::exception& e) {
    std::cout << "WriteObject: Problem parsing request: " << e.what() << "\n";
    return false;
  }

  //Parse path.
  //TODO (fux): Here different paths are needed:
  //paths_[0] = ../../data/users/
  //paths_[1] = ../../data/users/[username]/  (path to one folder)
  //paths_[2] = ../../data/users/[user]/      (path to shared folder
  //...                                       (additional shared folders)
  std::string path_to_object = path_ + "/" + path.substr(7, path.rfind("/")-7) 
    + ".json";

  //Read json
  std::cout << "Reading json\n";
  if (!func::demo_exists(path_to_object)) {
    std::cout << "Path: " << path_to_object << " don't exist!\n";
    return false;
  }
  nlohmann::json object = func::LoadJsonFromDisc(path_to_object);
  
  //Modify and write object.
  object[json["id"].get<std::string>()] = json;
  std::ofstream write(path_to_object);
  write << object;
  write.close();
  return true;
}

bool User::RestoreBackup(std::string request) {
  //Try to parse json and check if "world" and "backup" fields exist.
  nlohmann::json json;
  try {
    json = nlohmann::json::parse(request);
    if (json.count("world") == 0 || json.count("backup") == 0) 
      std::cout << "\"world\" or \"backup\" not found!" << std::endl;
  }
  catch (std::exception& e) {
    std::cout << "RestoreBackup: Problem parsing request: " << e.what() << "\n";
    return false;
  }
  //
  //Create path to selected backup and world.
  std::string path = path_ + "/backups/" + json["backup"].get<std::string>();
  std::string path_to_world = path_ + "/files/" + json["world"].get<std::string>();

  //Check if both paths exist.
  if (!func::demo_exists(path) || !func::demo_exists(path_to_world)) {
    std::cout << "Backup " << path << " doesn't exist! Or ";
    std::cout << "World " << path_to_world << " doesn't exist!" << std::endl;
    return false;
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
    return false;
  }
  return true;
}

bool User::DeleteBackup(std::string backup) {

  //Build path, then check if path exists.
  std::string path = path_ + "/backups/" + backup;
  if (!func::demo_exists(path)) {
    std::cout << "Backup " << path << " does not exist!" << std::endl;
    return false;
  }

  //Remove backup.
  fs::remove_all(path);
  return true;
}
