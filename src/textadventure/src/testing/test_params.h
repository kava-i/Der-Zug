/**
 * @author georgbuechner
 */

#ifndef DERZUG_SRC_TEXTADVENTURE_SRC_TESTING_TESTPARAMS_H
#define DERZUG_SRC_TEXTADVENTURE_SRC_TESTING_TESTPARAMS_H

#include <string>

class TestParameters{
  public: 
    static TestParameters *getInstance() {
      if (!instance)
        instance = new TestParameters;
      return instance;
    }

    std::string txtad_path() {
      return txtad_path_;
    }

    void set_txtad_path(std::string path) {
      txtad_path_ = path;
    }

  private:
    static TestParameters *instance;
    std::string txtad_path_;
    
    TestParameters() {
      txtad_path_ = "";
    }
};

#endif
