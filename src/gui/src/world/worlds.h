/**
 * @author fux
*/

#ifndef SRC_SERVER_WORLD_WORLDS_H
#define SRC_SERVER_WORLD_WORLDS_H

#include <filesystem>
#include <iostream>
#include <map>
#include <string>

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include "util/func.h"
#include "world.h"

class Worlds {
  public:
    Worlds(std::string base_path);
    ~Worlds();
    std::string GetPage(std::string path);

  private:
    int ports_;
    std::string base_path_;
    std::map<std::string, World*> worlds_;
    std::string ParseTemplate(nlohmann::json json);
};

#endif

