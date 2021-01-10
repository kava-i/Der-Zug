/**
 * @author fux
*/

#ifndef SRC_SERVER_WORLD_CATEGORY_H
#define SRC_SERVER_WORLD_CATEGORY_H

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

class Category : public Page {
  public: 
    Category(std::string base_path, std::string path);
    ~Category() {}

    nlohmann::json RenderPage(std::string path);

  private:
    void UpdateNodes();
};

#endif
