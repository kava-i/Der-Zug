/**
 * @author fux
*/

#ifndef SRC_SERVER_WORLD_PAGE_H
#define SRC_SERVER_WORLD_PAGE_H

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#include <inja/inja.hpp>
#include "util/func.h"

class Page {
  public:
    Page(std::string base_path, std::string path);
    virtual ~Page() {}

    virtual nlohmann::json RenderPage(std::string path) { return nlohmann::json(); }

  protected:
    std::string base_path_;
    std::string path_;
    std::string name_;
    std::map<std::string, std::string> nodes_;
    std::map<std::string, std::string> parents_;

    void GenerateParents();
    virtual void UpdateNodes() {};
};

#endif
