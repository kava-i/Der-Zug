#include <iostream>

#include "JanGeschenk/Webconsole.hpp"
#include "JanGeschenk/Webgame.hpp"
#include "game/game.h"
#include "eventmanager/sorted_context.h"
#include "tools/webcmd.h"


CGame *game;

class WebserverGame {
  private:
    std::string _name;
    std::string _password;
    std::string _id;
    Webconsole* _cout;

  public:
    WebserverGame(Webconsole *cout) {
      std::cout << "Starting....\n";

      _name = "";
      _password = "";
      _id = "";
      _cout = cout;
      _cout->write(Webcmd::set_color(Webcmd::color::ORANGE), "Name: ");
      _cout->flush();
    }

    const std::string &GetName() {
      return _name;
    }

    const std::string &GetID() {
      return _id;
    }

    void onmessage(std::string sInput, std::map<decltype(websocketpp::lib::
          weak_ptr<void>().lock().get()), WebserverGame*> *ptr, bool& global_shutdown) {
      if(_name=="") {
        _name=sInput;
        _cout->write(" " + sInput +"\n");
        _cout->flush();
        if(_name=="") {
          _cout->write(Webcmd::set_color(Webcmd::color::RED),"\nName: ");
          _cout->flush();
          return;
        }
        _cout->write(Webcmd::set_color(Webcmd::color::ORANGE),"\nPassword: ");
        _cout->flush();
        return;
      }

      if(_password=="") {
        _password = sInput;
        std::string str = "";
        for(size_t i=0; i<sInput.length(); i++) 
          str += "*";
        _cout->write(" " + str + "\n");
        _cout->flush();
        if(_password=="") {
          _cout->write("\nPassword: ");
          _cout->flush();
          return;
        }
        _id = game->checkLogin(_name,_password);
        if(_id=="") {
          _name = "";
          _password = "";
          _cout->write(Webcmd::set_color(Webcmd::color::RED), "Invalid Login please try again!",color::white, "\n\nName: ");
          _cout->flush();
          return;
        }
        std::cout << "Login data: " << _id << std::endl;
        sInput = game->startGame(sInput,_id, _cout);
        _cout->write(sInput);
        _cout->flush();
        return;
      } 

      std::list<std::string> lk;
      for(const auto &it : *ptr)
        lk.push_back(it.second->GetID());

      std::cout<<"Befor play: sInput: "<< sInput << " calling with id: " 
        << _id <<std::endl;
      std::string sOutput = game->play(sInput, _id, lk);
      if (sOutput == "[### end_game ###]") {
        sOutput = "Game closed by host";
        global_shutdown = true;
        std::cout << "Game is getting closed..." << std::endl;
      }
      else {
        _cout->write(sOutput);
        std::cout<<"Send the output to client: " << sOutput <<std::endl;
        _cout->flush();
      }
    }
};

int main(int x, char **argc) {
  CGame currentGame(argc[1]);
  game = &currentGame;
  Webgame<WebserverGame> gl;
  gl.run();
}

