/**
 * @author fux
*/

#ifndef SRC_SERVER_WORLD_AREA_H
#define SRC_SERVER_WORLD_AREA_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include "util/func.h"

#include "page.h"

class Area : public Page {
  public:
    Area(std::string base_path_, std::string path, nlohmann::json objects);
    ~Area() {}

    nlohmann::json RenderPage(std::string path);

  private:
    nlohmann::json objects_;

    nlohmann::json RenderObjectPage(std::string id);
    void UpdateNodes();
};

#endif
