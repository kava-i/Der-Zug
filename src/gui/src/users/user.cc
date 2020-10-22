/**
 * @author fux
*/
#include "user.h"
#include "util/func.h"

namespace fs = std::filesystem;

User::User(std::string name, std::string pw, std::string path, std::string 
    path_backup, std::vector<std::string> cats) : username_(name), 
    path_(path), path_backup_(path_backup), categories_(cats) {
  password_ = pw;
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
  //Create initial json with username.
  nlohmann::json worlds = nlohmann::json({{"username", username_}});

  //Add all dictionaries of this user to json.
  for (auto& p : fs::directory_iterator(path_)) {
    if (p.path().stem() != "user")
      worlds["worlds"].push_back(p.path().stem());
  }
    
  //Parse overview page. 
  inja::Environment env;                                                                  
  inja::Template temp = env.parse_template("web/overview_template.html");
  return env.render(temp, worlds);
}

std::string User::GetWorld(std::string world) {
  //Create initial json with world and all categories.
  nlohmann::json j = nlohmann::json({{"world", world}, {"categories", 
      categories_}});

  //Create dictionaries for all categories, if they do not exist.
  for (auto cat : categories_)
    fs::create_directory(path_+"/"+world+"/"+cat);

  //Parse world page. 
  inja::Environment env;                                                                  
  inja::Template temp = env.parse_template("web/in_world_template.html");
  return env.render(temp, j);
}

std::string User::GetBackups(std::string world) {
  //Get all backups for this world.
  std::vector<std::string> backups;
  for (auto& p  : fs::directory_iterator(path_backup_)) {
    std::string backup = p.path().filename();
    if (backup.find(world) == 0) {
      backups.push_back(backup);
    }
  }

  //Parse backup page. 
  nlohmann::json j = nlohmann::json({{"world", world}, {"backups", backups}});
  inja::Environment env;                                                                  
  inja::Template temp = env.parse_template("web/backups_template.html");
  return env.render(temp, j);
}

std::string User::GetCategory(std::string world, std::string category) {
  //Build path to category and create initial json.
  std::string path = path_+"/"+world+"/"+category;
  nlohmann::json j_category = nlohmann::json({{"world", world}, 
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
  //Create path and trying to load into json 
  std::string path = path_+"/"+world+"/"+category+"/"+sub+".json";
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
  nlohmann::json overview = nlohmann::json({{"world", world}, {"category", category},
        {"sub", sub}, {"objects", nlohmann::json::array()}});
  
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
  //Create path and trying to load into json 
  std::string path = path_+"/"+world+"/"+category+"/"
    +sub+".json";
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
    int num = std::stoi(obj.substr(obj.length()-1))-1;
    if (objects.size() > num)
      overview["json"] = objects[num];
  }
  overview["header"] = nlohmann::json({{"world", world}, {"category", category},
      {"sub", sub}, {"obj",obj}});

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

void User::SafeUser() const {
  try { 
    fs::create_directory(path_);
  }
  catch(...) {}
  std::string path = path_ + "/user.json";  
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
  return user;
}

bool demo_exists(const fs::path& p, fs::file_status s = fs::file_status{}) {
  if (fs::status_known(s) ? fs::exists(s) : fs::exists(p)) 
    return true;
  else
    return false;
}

bool User::CreateBackup(std::string world) {
  std::string path = path_ + "/" + world;
  
  if (!demo_exists(path)) return false;

  //Create backup name ([word]_[YYYY-mm-dd_HH-MM-ss])
  time_t now;
  std::time(&now);
  char buf_human[sizeof("YYYY-mm-dd_HH-MM-ss")];
  strftime(buf_human, sizeof(buf_human), "%F_%T", localtime(&now));
  std::string path_backup = path_backup_ + "/" + world + "_" + buf_human;
  std::replace(path_backup.begin(), path_backup.end(), ':', '-');

  //Check if folder for this user already exists
  if (!demo_exists(path_backup_))
    fs::create_directory(path_backup_);

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
  try {
    json = nlohmann::json::parse(request);
    if (json.count("json") == 0 || json.count("path") == 0) 
      std::cout << "\"json\" or \"path\" not found!" << std::endl;
  }
  catch (std::exception& e) {
    std::cout << "WriteObject: Problem parsing request: " << e.what() << "\n";
    return false;
  }

  //Parse path.
  std::cout << "Path:" << std::endl;
  std::cout << json["path"] << std::endl;
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
  std::string path = path_backup_ + "/" + json["backup"].get<std::string>();
  std::string path_to_world = path_ + "/" + json["world"].get<std::string>();

  //Check if both paths exist.
  if (!demo_exists(path) || !demo_exists(path_to_world)) {
    std::cout << "Backup " << path << " doesn't exist! Or ";
    std::cout << "World " << path_to_world << " doesn't exist!" << std::endl;
    return false;
  }

  //Restore backup.
  const auto copy_options = fs::copy_options::update_existing
                          | fs::copy_options::recursive;
  try {
    std::uintmax_t n = fs::remove_all(path_to_world);  //Delete original
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
  std::string path = path_backup_ + "/" + backup;
  if (!demo_exists(path)) {
    std::cout << "Backup " << path << " does not exist!" << std::endl;
    return false;
  }

  //Remove backup.
  std::uintmax_t n = fs::remove_all(path);
  return true;
}
