/**
 * @author fux
*/
#include "user.h"
#include "util/func.h"

namespace fs = std::filesystem;

User::User(std::string name, std::string pw, std::string path, std::vector
    <std::string> cats) : username_(name), path_(path), categories_(cats) {
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
  nlohmann::json worlds;
  worlds["username"] = username_;
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
  nlohmann::json j = nlohmann::json({{"world", world}, {"categories", categories_}});
  for (auto cat : categories_)
    fs::create_directory(path_+"/"+world+"/"+cat);

  //Parse overview page. 
  inja::Environment env;                                                                  
  inja::Template temp = env.parse_template("web/in_world_template.html");
  return env.render(temp, j);
}

std::string User::GetCategory(std::string world, std::string category) {
  std::string path = path_+"/"+world+"/"+category;
  nlohmann::json j_category = nlohmann::json({{"world", world}, 
      {"category", category}});
  if (category == "config") {
    std::ifstream read(path+".json");
    nlohmann::json j;
    read >> j;
    read.close();
    j_category["json"] = j;
    inja::Environment env;
    inja::Template temp = env.parse_template("web/in_json_template.html");
    return env.render(temp, j_category);
  }

  j_category["sub_categories"] = nlohmann::json::array();
  for (auto p : fs::directory_iterator(path+"/")) 
    j_category["sub_categories"].push_back(p.path().stem());

  inja::Environment env;
  inja::Template temp = env.parse_template("web/in_category_template.html");
  return env.render(temp, j_category);
}

std::string User::GetObjects(std::string world, std::string category, 
      std::string sub) {
  nlohmann::json objects;
  std::string path = path_+"/"+world+"/"+category+"/"
    +sub+".json";
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
  nlohmann::json objects;
  std::string path = path_+"/"+world+"/"+category+"/"
    +sub+".json";
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
  if (objects.is_array() == false) {
    if (objects.count(obj) > 0)
      overview = objects[obj];
  }
  else {
    int num = std::stoi(obj.substr(obj.length()-1))-1;
    if (objects.size() > num)
      overview["json"] = objects[num];
  }
  overview["header"] = nlohmann::json({{"world", world}, {"category", category},
      {"sub", sub}, {"obj",obj}});


  inja::Environment env;
  inja::Template temp;
  if (category == "rooms") {
    inja::Template description_template = env.parse_template("web/text_template.html");
    env.include_template("web/temp_description", description_template);

    inja::Template object_header_template = env.parse_template("web/object_header_template.html");
    env.include_template("web/temp_header", object_header_template);

    temp = env.parse_template("web/in_room_template.html");
  }
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
