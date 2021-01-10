/**
 * @author fux
*/

#ifndef SRC_SERVER_WORLD_WORLD_H
#define SRC_SERVER_WORLD_WORLD_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>

#include "page/page.h"
#include "util/func.h"

class World {
  public:
    World() {}
    World(std::string base_path, std::string path, int port);
    ~World();

    int port();
    nlohmann::json GetPage(std::string path);

  private:
    std::string base_path_;
    std::string path_;
    int port_;
    std::map<std::string, Page*> paths_;
    std::vector<std::string> short_paths_;

    void InitializePaths(std::string path);
    void UpdateShortPaths();
};

#endif
