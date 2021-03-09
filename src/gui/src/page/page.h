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
#include "util/error_codes.h"
#include "util/func.h"

/**
 * Base class for pages. 
 * Class is abstract and never instaciated. 
 * Also storing all parents and child-nodes.
 */
class Page {
  public:
    // constructer/ destructor:
    Page(std::string base_path, std::string path);
    virtual ~Page() {}

    // getter:
    virtual std::string category() const { return ""; }
    std::string name() const;

    // public methods:
    
    /**
     * Add new element:  directory, file or object.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @return ErrorCode indicating success/ error.
     */
     virtual ErrorCodes AddElem(std::string, std::string) { return ErrorCodes::FAILED; }

    /**
     * Delete element:  directory, file or object.
     * @param[in] path to category or area.
     * @param[in] name of directory, file or object.
     * @return ErrorCode indicating success/ error.
     */
     virtual ErrorCodes DelElem(std::string, std::string) { return ErrorCodes::FAILED; }

    /**
     * Undo delete element: restore deleted directory, file, object.
     * Each derived class has it's own way of storing deleted data.
     */
     virtual ErrorCodes UndoDelElem() { return ErrorCodes::FAILED; }

    /**
     * Creates data for page: json-data & path to matching templae.
     * Used by category and area, however not for objects.
     * @param[in] path path to target page.
     * @return json with information.
     */
    virtual nlohmann::json CreatePageData(std::string path);

  protected:
    // member variables:
    std::string base_path_;
    std::string path_;
    std::string category_;
    std::string name_;

    /**
     * Child nodes.
     * key: url-path, value: child-name.
     */
    std::map<std::string, std::string> child_nodes_;

    /**
     * Parent nodes.
     * key: url-path, value: parent-name.
     */
    std::map<std::string, std::string> parent_nodes_;

    // private methods:

    /**
     * Generates all parents. (Identical for category and area.)
     */
    void GenerateParentNodes();

    /**
     * Updates all child-nodes. (Virtual add different for category/ area.)
     */
    virtual void GenerateChildNodes() {}
    
    /**
     * Get template with list of (eventually multiple) objects.
     * It is expected, that only one element exists in object. However
     * some templates might contain more than one element from the beginning.
     * @param[in, out] template_area of area to create.
     * @return boolean indicatiing success.
     */
    bool GetObjectsFromTemplate(nlohmann::json& template_area);

    /**
     * Create json from template.
     * Concerning an object, there is a good chance, that some entries can be
     * made just from nowing the id (f.e that name might often be identical to
     * the id, only with an upper-case letter at the beginning. Thus adding the
     * id and name field (if not already set by template!) might turn out quite
     * convinient for the user.
     * @param[in, out] object to create.
     * @param name given by user.
     * @return boolean indicatiing success.
     */
    bool GetObjectFromTemplate(nlohmann::json& object, std::string name);
};

#endif
