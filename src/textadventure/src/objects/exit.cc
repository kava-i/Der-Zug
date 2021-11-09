#include "exit.h"

std::string CExit::target() { 
    return target_; 
}
std::string CExit::prep() { 
    return prep_; 
}
bool CExit::hidden() {
  return hidden_;
}

void CExit::set_hidden(bool hidden) {
  hidden_ = hidden;
}
