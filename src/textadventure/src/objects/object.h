#ifndef COBJECT_H
#define COBJECT_H

#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

#include "concepts/text.h"
#include "nlohmann/json_fwd.hpp"
#include "tools/func.h"

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
    std::map<std::string, nlohmann::json> handler_;     ///< handlers to be added.
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
      std::vector<nlohmann::json> handler = jAttributes.value("handlers", std::vector<nlohmann::json>());
      int counter = 0;
      for (const auto& it : handler) {
        if (it.contains("key") && handler_.count(it["key"]) == 0) {
          handler_[it["key"]] = it;
          std::cout << "Added handler: " << it["key"] << ": " << it.dump() << std::endl;
        }
        else 
          handler_[std::to_string(counter++)] = it;
      }
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
      std::vector<nlohmann::json> handler;
      for (const auto& it : handler_) {
        std::cout << "Added handler: " << it.second.dump() << std::endl;
        handler.push_back(it.second);
      }
      return handler;
    }

    std::string image() {
      return image_;
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
    void set_image(std::string file_name) {
      image_ = file_name;
    }
    
    /// set new music.
    void set_music(std::string file_name) {
      music_ = file_name;
    }

    ///Removes handler:
    void RemoveHandler(std::string key) {
      std::cout << "Object::RemoveHandler: " << key << std::endl;
      handler_.erase(key);
      std::cout << "Object::RemoveHandler: removed " << std::endl;
    }
};

#endif
