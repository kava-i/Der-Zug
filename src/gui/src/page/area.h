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
     * Add new object.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes AddElem(std::string path, std::string name);

    /**
     * Delete object.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @return ErrorCode indicating success/ error.
     */
    ErrorCodes DelElem(std::string path, std::string name);

    /**
     * Restore deleted object.
     * Before deleting object, the object will be stored, and can be recreated.
     */
    ErrorCodes UndoDelElem();

    /**
     * Calls base-class function for area and CreateObjectPageData for opject.
     * @param[in] path to area or object.
     * @return json with information.
     */
    nlohmann::json CreatePageData(std::string path);

  private:
    // member variables:
    nlohmann::json objects_;
    nlohmann::json last_deleted_obj_;

    // private methods:
    
    /**
     * Creates data for requested object.
     * Gets object from json and adds parent-nodes.
     * @param[in] id of requested object.
     * @return json with information.
     */
    nlohmann::json CreateObjectPageData(std::string id);

    void UpdateNodes();

    /**
     * Checks whether the the area, or one of it objects is referred to.
     * @param[in] path selected path (might be path to object or to area itself)
     * @return true if path is not path to area, false otherwise.
     */
    bool IsObject(std::string path);
};

#endif
