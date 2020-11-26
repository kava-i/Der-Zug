#include <iostream>
#include <fstream>

#include <httplib.h>
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

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

std::string GetPage(std::string path) {
  //Read loginpage and send
  std::ifstream read(path);
  if (!read) {
    std::cout << "Wrong file passed: " << path << std::endl;
    return "";
  }
  std::string page( (std::istreambuf_iterator<char>(read) ),
                    std::istreambuf_iterator<char>()     );
  //Delete file-end marker
  page.pop_back();
  return page;
}

int main(int x, char **argc) {
  CGame currentGame(argc[1]);
  game = &currentGame;
  Webgame<WebserverGame> gl;
  int port = 9002;
  if (x>2)
    port = std::stoi(argc[2]);

  //Open httpserver to serve main-page
  std::thread t1([&]() {
    httplib::Server srv;
    srv.Get("/", [&](const httplib::Request& req, httplib::Response& resp)  {
      inja::Environment env;
      inja::Template temp = env.parse_template("index.html");
      resp.set_content(env.render(temp, nlohmann::json({{"port",port+1}})), 
          "text/html"); });
    srv.listen("0.0.0.0", port);
  });

  std::thread t2([&]() {
    gl.run(port+1);
  });
  t1.join(); 
  t2.join(); 
}
