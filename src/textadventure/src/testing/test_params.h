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

    std::string txtad_player() {
      return txtad_player_;
    }

    void set_txtad_player(std::string player) {
      txtad_player_ = player;
    }
  private:
    static TestParameters *instance;
    std::string txtad_path_;
    std::string txtad_player_;
    
    TestParameters() {
      txtad_path_ = "";
      txtad_player_ = "";
    }
};

#endif
