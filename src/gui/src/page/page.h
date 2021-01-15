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

    // public methods:
    
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
    virtual void GenerateChildNodes() {};
};

#endif
