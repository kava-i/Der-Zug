#ifndef CEXIT_H
#define CEXIT_H 

#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

#include "object.h"

class CExit : public CObject {
  private:
    std::string target_; 
    std::string prep_;
    bool hidden_;

  public:
    CExit(std::string trarget, nlohmann::json json_atts, CPlayer* p) : CObject(json_atts, p, "exit") {
      target_ = trarget;
      prep_ = json_atts.value("prep", "");
      hidden_ = json_atts.value("hidden", 0);
    }

    // getter
    std::string target();
    std::string prep();
    bool hidden();

    // setter
    void set_hidden(bool hidden);
};

#endif 
