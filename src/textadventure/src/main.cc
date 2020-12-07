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
httplib::Server srv;

class WebserverGame {
  private:
    std::string name_;
    std::string password_;
    std::string id_;
    Webconsole* cout_;

  public:
    WebserverGame(Webconsole *cout) {
      std::cout << "Starting....\n";

      name_ = "";
      password_ = "";
      id_ = "";
      cout_ = cout;
    }

    const std::string &GetName() {
      return name_;
    }

    const std::string &GetID() {
      return id_;
    }

    void send(std::string msg) {
      cout_->write(msg);
      cout_->flush();
    }

    void onmessage(std::string sInput, std::map<decltype(websocketpp::lib::
          weak_ptr<void>().lock().get()), WebserverGame*> *ptr, bool& global_shutdown) {
      if(name_=="") {
        name_=sInput;
        send(" " + sInput + "\n");
        if(name_=="") {
          cout_->write(Webcmd::set_color(Webcmd::color::RED), "\nName: ");
          cout_->flush();
          return;
        }
        cout_->write(Webcmd::set_color(Webcmd::color::ORANGE), "\nPassword: ");
        cout_->flush();
        return;
      }

      if(password_=="") {
        password_ = sInput;
        std::string str = "";
        for(size_t i=0; i<sInput.length(); i++) 
          str += "*";
        send(" " + str + "\n");
        if(password_=="") {
          send("\nPassword: ");
          return;
        }
        id_ = game->checkLogin(name_,password_);
        if(id_=="") {
          name_ = "";
          password_ = "";
          cout_->write(Webcmd::set_color(Webcmd::color::RED), 
              "Invalid Login please try again!", color::white, "\n\nName: ");
          cout_->flush();
          return;
        }
        std::cout << "Login data: " << id_ << std::endl;
        sInput = game->startGame(sInput,id_, cout_);
        send(sInput);
        return;
      } 

      std::list<std::string> lk;
      for(const auto &it : *ptr)
        lk.push_back(it.second->GetID());

      std::cout<<"Befor play: sInput: "<< sInput << " calling with id: " 
        << id_ <<std::endl;
      std::string sOutput = game->play(sInput, id_, lk);
      if (sOutput == "[### end_game ###]") {
        sOutput = "Game closed by host";
        global_shutdown = true;
        srv.stop();
        std::cout << "Game is getting closed..." << std::endl;
      }
      else {
        send(sOutput);
        std::cout<<"Send the output to client: " << sOutput <<std::endl;
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

std::string GetImage(std::string path, std::string image) {
  path = path + "images" + image + ".jpg";
  std::ifstream f(path, std::ios::in|std::ios::binary|std::ios::ate);    
  if (!f.is_open()) {
    std::cout << "file could not be found: " << path << std::endl;    
    return "";
  }
  FILE* file_stream = fopen(path.c_str(), "rb");    
  std::vector<char> buffer;    
  fseek(file_stream, 0, SEEK_END);    
  long length = ftell(file_stream);    
  rewind(file_stream);    
  buffer.resize(length);    
  length = fread(&buffer[0], 1, length, file_stream);    
      
  std::string s(buffer.begin(), buffer.end());    
  return s;
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

    //Function to serve main app
    srv.Get("/", [&](const httplib::Request& req, httplib::Response& resp)  {
      inja::Environment env;
      inja::Template temp = env.parse_template("index.html");
      resp.set_content(env.render(temp, nlohmann::json({{"port",port+1}})), 
          "text/html"); 
      });
    //Function to serve images
    srv.Get("(.*).jpg", [&](const httplib::Request& req, httplib::Response& resp)  {
        resp.set_content(GetImage(argc[1], req.matches[1]), "image/jpg"); });

    //Start main loop.
    srv.listen("0.0.0.0", port);
    return 0;
  });

  std::thread t2([&]() {
      gl.run(port+1);
  });
  t1.join(); 
  t2.join(); 
}
