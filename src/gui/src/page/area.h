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

    // getter:
    std::string category() const; 

    // public methods:

    /**
     * Modify an object. 
     * Stores old-object until verfyfied, that game is still running. 
     * @param[in] path to object, which shall be modified.
     * @param[in] modified_object which will replace old object.
     */
    ErrorCodes ModifyObject(std::string path, std::string name, nlohmann::json modified_object);

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
     * Restore backup object: restore deleted/ modified directory, file, object.
     * Each derived class has it's own way of storing deleted data.
     */
    ErrorCodes RestoreBackupObj();

    /**
     * Calls base-class function for area and CreateObjectPageData for opject.
     * @param[in] path to area or object.
     * @return json with information.
     */
    nlohmann::json CreatePageData(std::string path);

  private:
    // member variables:
    nlohmann::json objects_;
    nlohmann::json backup_obj_;

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
