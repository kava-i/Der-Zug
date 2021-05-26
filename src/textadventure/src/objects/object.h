#ifndef COBJECT_H
#define COBJECT_H

#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

#include "concepts/text.h"

class CPlayer;

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
    std::vector<nlohmann::json> handler_;     ///< handlers to be added.
    std::string image_; 
    std::string music_;

public:

    /**
    * Constructor.
    * @param[in] jAttributes json containing all attributes of object.
    */
    CObject(nlohmann::json jAttributes, CPlayer* p) {
      id_ = jAttributes.value("id", "");
      name_ = jAttributes.value("name", "");
      text_ = new CText(jAttributes.value("description", nlohmann::json::parse("{}") ), p);
      handler_ = jAttributes.value("handlers", std::vector<nlohmann::json>());
      image_ = jAttributes.value("image", "");
      music_ = jAttributes.value("music", "");
    }

    virtual ~CObject() { delete text_; }

    // *** GETTER *** //

    ///Return id of object
    std::string id() {
        return id_;
    }

    ///Return name of object.
    std::string name() {
        return name_;
    }

    ///Return description by calling print-function from text class.
    std::string text() {
        return text_->print();
    }

    ///Return list of handler
    std::vector<nlohmann::json> handler() {
        return handler_;
    }

    std::string image() {
      return music_;
    }

    std::string music() {
      return music_;
    }

    ///Set id of object.
    void set_id(std::string sID) {
        id_ = sID;
    }

    ///Set name of object.
    void set_name(std::string sName) {
        name_ = sName;
    }

    /// set media.
    void image(std::string file_name) {
      image_ = file_name;
    }
    
    /// set new music.
    void music(std::string file_name) {
      music_ = file_name;
    }
};

#endif
