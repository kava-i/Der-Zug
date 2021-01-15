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

    // public methods:

    /**
     * Calls base-class function for area and CreateObjectPageData for opject.
     * @param[in] path to area or object.
     * @return json with information.
     */
    nlohmann::json CreatePageData(std::string path);

  private:
    // member variables:
    nlohmann::json objects_;

    // private methods:
    
    /**
     * Creates data for requested object.
     * Gets object from json and adds parent-nodes.
     * @param[in] id of requested object.
     * @return json with information.
     */
    nlohmann::json CreateObjectPageData(std::string id);
    void UpdateNodes();
};

#endif
