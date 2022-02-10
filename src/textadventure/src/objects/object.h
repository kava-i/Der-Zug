#ifndef COBJECT_H
#define COBJECT_H

#include <exception>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

#include "concepts/text.h"
#include "nlohmann/json_fwd.hpp"
#include "tools/func.h"

#define cRED "\033[1;31m"
#define cCLEAR "\033[0m"

class CPlayer;
class CListener;

/**
* Class from which every other object derives, as every object has at least 
* a name and an id. Later maybe information on grammar will be added.
*/
class CObject 
{
protected: 
    std::string id_;      ///< unique identifier for object.
    std::string name_;    ///< name of object.
    CText* text_;          ///< text class holdes description
    std::map<std::string, CListener*> listeners_;     ///< handlers to be added.
    std::string image_; 
    std::string music_;

public:

    /**
    * Constructor.
    * @param[in] jAttributes json containing all attributes of object.
    */
    CObject(nlohmann::json jAttributes, CPlayer* p, std::string location);

    virtual ~CObject() { delete text_; }

    // getter
    std::string id();
    std::string name();
    ///Return description by calling print-function from text class.
    std::string text(); 
    ///Return list of handler
    std::vector<CListener*> listeners(); 
    std::string image(); 
    std::string music(); 

    // setter 
    void set_id(std::string sID); 
    void set_name(std::string sName); 
    void set_image(std::string file_name); 
    void set_music(std::string file_name); 

    // functions
    
    /**
     * Removes handler from object.
     * @param[in] key
     */
    void RemoveHandler(std::string key); 
};

#endif
