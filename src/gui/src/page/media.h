/**
 * @author fux
*/

#ifndef SRC_SERVER_WORLD_MEDIA_H
#define SRC_SERVER_WORLD_MEDIA_H

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

class Media : public Page {
  public:
    Media(std::string base_path_, std::string path, std::string type, std::string etension);
    ~Media() {}

    /**
     * Add new object.
     * @param[in] path to category or area.
     * @param[in] id of object to add.
     * @param[in] infos f.e name of start-room, when adding player.
     * @return ErrorCode indicating success/ error.
     */
    //ErrorCodes AddElem(std::string path, std::string id, nlohmann::json infos);

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
    //ErrorCodes RestoreBackupObj();

    /**
     * Calls base-class function.
     ** @param[in] path to area or object.
     * @return json with information.
     */
    nlohmann::json CreatePageData(std::string path);

  private:
    const std::string type_;  ///< might be f.e. image, music, etc.
    const std::string extension_;  ///< the file extension (mp3, jpeg, etc.)
    std::string backup_file_;
};

#endif
