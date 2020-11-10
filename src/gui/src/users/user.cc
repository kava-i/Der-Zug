/**
 * @author fux
*/
#include "user.h"
#include "util/func.h"
#include <exception>
#include <filesystem>

namespace fs = std::filesystem;

User::User(std::string name, std::string pw, std::string path, std::vector
    <std::string> cats) : username_(name), path_(path), categories_(cats) {
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
    std::vector<std::string> locations, std::vector
    <std::string> cats) : username_(name), path_(path), categories_(cats) {
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
  worlds["worlds"] = nlohmann::json::array();
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
  //Check if path to given world can be found
  if (!func::demo_exists(path_+path))
    return "";

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

std::string User::GetCategory(std::string path, std::string world, 
    std::string category) {
  std::cout << "GetCategory" << std::endl;
  
  //Create path, inja Environment and initial json.
  path = path_ + path;
  inja::Environment env;
  nlohmann::json j_category = nlohmann::json({{"user", username_}, 
      {"world", world}, {"category", category}});

  //Directly parse config page.
  if (category == "config") {
    nlohmann::json config_json;
    if (!func::LoadJsonFromDisc((path+".json"), config_json))
      return "";
    j_category["json"] = config_json;
    inja::Template temp = env.parse_template("web/object_templates/config.html");
    return env.render(temp, j_category);
  }

  //Check if path to given category exists 
  if (!func::demo_exists(path)) {
    std::cout << "Path: " << path << " does not exist!" << std::endl;
    return "";
  }
  
  //Add all files in category to json and parse json.
  j_category["sub_categories"] = nlohmann::json::array();
  for (auto p : fs::directory_iterator(path+"/")) 
    j_category["sub_categories"].push_back(p.path().stem());
  inja::Template temp = env.parse_template("web/in_category_template.html");
  return env.render(temp, j_category);
}

std::string User::GetObjects(std::string path, std::string world, std::string 
    category, std::string sub) {
  std::cout << "GetObjects" << std::endl;
  //Create path and trying to load into json 
  path = path_ + path + ".json";
  nlohmann::json objects;
  try{ 
    std::ifstream read(path);
    read>>objects;
    read.close();
  }
  catch(...) { 
    std::cout << "file: " << path << " not found.";
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

std::string User::GetObject(std::string path, std::string world, std::string 
    category, std::string sub, std::string obj) {
  //Create path and trying to load into json 
  path = path_ + path.substr(0, path.rfind("/")) + ".json";
  nlohmann::json objects;
  try{ 
    std::ifstream read(path);
    read>>objects;
    read.close();
  }
  catch(...) { 
    std::cout << "file: " << path << "not found.";
    return "File not found.";
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

  //Parse standard templates for descriptions and header and footer included 
  //in every object.
  inja::Template description = env.parse_template(
      "web/object_templates/description.html");
  env.include_template("web/object_templates/temp_description", description);

  inja::Template room_description = env.parse_template(
      "web/object_templates/room_description.html");
  env.include_template("web/object_templates/temp_room_description", 
      room_description);

  inja::Template dead_description = env.parse_template(
      "web/object_templates/dead_description.html");
  env.include_template("web/object_templates/temp_dead_description", 
      dead_description);

  inja::Template use_description = env.parse_template(
      "web/object_templates/use_description.html");
  env.include_template("web/object_templates/temp_use_description", 
      use_description);

  inja::Template pages = env.parse_template("web/object_templates/pages.html");
  env.include_template("web/object_templates/temp_pages", pages);

  inja::Template header = env.parse_template("web/object_templates/header.html");
  env.include_template("web/object_templates/temp_header", header);
  inja::Template footer = env.parse_template("web/object_templates/footer.html");
  env.include_template("web/object_templates/temp_footer", footer);

  //Parse different objects.
  try {
    temp = env.parse_template("web/object_templates/"+category+".html");
  }
  catch (std::exception& e) {
    std::cout << "Problem parsing object html: " << e.what() << std::endl;
    return "File not found.";
  }
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

    //Copy default config to world.
    fs::copy("../../data/default_jsons/config.json", path); 

    //Copy default room to world.
    fs::copy("../../data/default_jsons/test.json", path + "/rooms/"); 

    //Copy default player-file to world.
    fs::copy("../../data/default_jsons/players.json", path + "/players/"); 
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

std::string User::AddFile(std::string path, std::string name) {
  path = path_ + path;
  //Check if path exists
  if (!func::demo_exists(path)) 
    return "Path not found.";
  //Check for wrong format
  if (name.find("/") != std::string::npos)
    return "Wrong format.";
  //Replace space by underscore and check if file already exists.
  std::replace(name.begin(), name.end(), ' ', '_');
  if (func::demo_exists(path + "/" + name + ".json"))
    return "File already exists.";

  //Get Category from string
  std::string category = path.substr(path.rfind("/") + 1);
  std::cout << category << std::endl;

  //Check for not supported categories
  if (category == "players")
    return "Not supported category.";

  nlohmann::json file;
  if (category == "defaultDialogs")
    file = nlohmann::json::array();
  else 
    file = nlohmann::json::object();
  
  try {
    std::cout << "Writing: " << path + "/" + name + ".json" << std::endl;
    std::ofstream write(path + "/" + name + ".json");
    write << file;
    write.close();
  }
  catch (std::exception& e) {
    std::cout << "Adding new file failed!: " << e.what() << std::endl;
    return "Adding file failed!";
  }
  return "";
}

std::string User::AddNewObject(std::string path, std::string id) {
  std::string full_path = path_ + path + ".json";
  std::cout << "AddNewObject: " << path << std::endl;
  //Check if path exists
  if (!func::demo_exists(full_path)) 
    return "Path not found.";
  //Check for wrong format
  if (id.find("/") != std::string::npos)
    return "Wrong format.";
  //Replace space by underscore and check if file already exists.
  std::replace(id.begin(), id.end(), ' ', '_');
  if (func::demo_exists(full_path + "/" + id + ".json"))
    return "Object already exists.";

  //Get Category
  std::vector<std::string> elements = func::Split(path, "/");
  if (elements.size() < 2)
    return "Category could not be parsed.";
  std::string category = elements[elements.size()-2];
  
  //Load empty object of category
  nlohmann::json request;
  request["path"] = path+"/"+id;
  if (category == "defaultDialogs") 
    request["json"] = nlohmann::json::array();
  else if (category == "defaultDescriptions")
    request["json"] = nlohmann::json::array();
  else 
    request["json"] = nlohmann::json{{"id", id}};
  try {
    if (WriteObject(request.dump()) == false)
      return "Problem with writing object.";
  }
  catch (std::exception& e) {
    std::cout << "Problem writing new object: " << e.what() << std::endl;
    return "Problem writing new object.";
  }
  return "";
}

bool User::WriteObject(std::string request, bool force) {
  //Get values from request
  nlohmann::json json;
  std::string path;
  std::cout << "reading data." << std::endl;
  try {
    nlohmann::json j = nlohmann::json::parse(request);
    json = j["json"];
    path = j["path"];
    std::cout << "Json:"  << json << std::endl; 
  }
  catch (std::exception& e) {
    std::cout << "WriteObject: Problem parsing request: " << e.what() << "\n";
    return false;
  }
  std::cout << "Reading object." << std::endl;

  //Parse path, read json and create doublicate (backup)
  std::string path_to_object = path_ + path.substr(0, path.rfind("/")) + ".json";
  nlohmann::json object;
  if (!func::LoadJsonFromDisc(path_to_object, object))
    return false;
  nlohmann::json obj_backup = object;
  std::cout << "Adding to object." << std::endl;
  
  //Modify and write object.
  if (json.is_array() == true && object.is_array() == true)
    object.push_back(json);
  else if (json.is_array() == true)
    object[std::to_string(object.size())] = json;
  else {
    std::cout << "Adding now!" << std::endl;
    object[json["id"].get<std::string>()] = json;
  }

  std::cout << "writing data." << std::endl;
  std::ofstream write(path_to_object);
  write << object;
  write.close();

  std::cout << "checking game." << std::endl;
  //Check if game is still running and return true if yes, are forcing is true
  if (CheckGameRunning(path) == true || force == true)
    return true;

  std::cout << "reading data." << std::endl;
  //If not and forcing to write is set to false
  std::ofstream write_backup(path_to_object);
  write_backup << obj_backup;
  write_backup.close();
  return false;
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



bool User::CreateBackup(std::string user, std::string world) {
  std::string path = path_ + "/" + user + "/files/" + world;
  
  if (!func::demo_exists(path)) return false;

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
    return false;
  }
  return true;
}


bool User::RestoreBackup(std::string user, std::string backup) {
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

bool User::DeleteBackup(std::string user, std::string backup) {

  //Build path, then check if path exists.
  std::string path = path_+"/"+user+"/backups/"+backup;
  if (!func::demo_exists(path)) {
    std::cout << "Backup " << path << " does not exist!" << std::endl;
    return false;
  }

  //Remove backup.
  fs::remove_all(path);
  return true;
}

bool User::CheckGameRunning(std::string path) {
  std::cout << "!!! CheckGameRunning !!!" << std::endl;

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
    std::string test_p = command + " -p " + it.key() + 
      " > ../../data/users/"+user+"/logs/"+world+".txt";
    if (system(test_p.c_str()) != 0) 
      success = false;
    std::cout << "Running with player \"" << it.key() << "\": " << success << std::endl;
  }
  return success;
}
