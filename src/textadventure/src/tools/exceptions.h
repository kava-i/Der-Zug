#ifndef SRC_TOOLS_EXCEPTIONS_H_
#define SRC_TOOLS_EXCEPTIONS_H_

#include <exception>
#include <string>

class WorldFactoryException : public std::exception {
  private: 
    std::string _msg;

  public: 
    WorldFactoryException(std::string msg) : _msg("WorldFactoryException: " + msg) {}
    const char* what() const noexcept {
      return _msg.c_str();
    }
};

#endif

