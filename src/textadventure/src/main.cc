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
    std::string sign_in_up_;
    Webconsole* cout_;

  public:
    WebserverGame(Webconsole *cout) {
      std::cout << "Starting....\n";
      name_ = "";
      password_ = "";
      id_ = "";
      sign_in_up_ = "";
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

    void SignInUp(std::string in) {
      if (sign_in_up_ == "") {
        std::cout << "Choosing login/ register";
        if (in == "l" || in == "r" || in == "login" || in == "register") {
          sign_in_up_ = in.substr(0,1);
          cout_->write(Webcmd::set_color(Webcmd::color::ORANGE), "\nName: ");
          cout_->flush();
          std::cout << "Set sign_in_up_ to " << in.substr(0,1) << std::endl;
        }
        else  {
          std::cout << "Wrong input" << std::endl;
          cout_->write(Webcmd::set_color(Webcmd::color::RED), 
              "Invalid option please try again!", color::white, "\n\nlogin/register (l/r): ");
          cout_->flush();
        }
      }
      else {
        std::cout << "On to getting credentials" << std::endl;
        if(name_=="") {
          name_ = in ;
          send(" " + in + "\n");
          cout_->write(Webcmd::set_color(Webcmd::color::ORANGE), "Password: ");
          cout_->flush();
          return;
        }

        if(password_ == "") {
          password_ = in;
          std::string str(in.length(), '*');
          send(" " + str + "\n");
          id_ = game->checkLogin(name_,password_);
          if(id_== "") {
            name_ = "";
            password_ = "";
            cout_->write(Webcmd::set_color(Webcmd::color::RED), 
                "Invalid Login please try again!", color::white, "\n\nName: ");
            cout_->flush();
          }
          else {
            std::cout << "Login data: " << id_ << std::endl;
            if (sign_in_up_ == "l") 
              send("Logged in as " + id_ + "\n\n");
            else
              send("Registered as " + id_ + "\n\n");
            send(game->startGame(in, id_, cout_));
          }
        }
      }
    }

    void onmessage(std::string sInput, std::map<decltype(websocketpp::lib::
          weak_ptr<void>().lock().get()), WebserverGame*> *ptr, bool& global_shutdown) {

      //Check for login/register-phase
      if (sign_in_up_ == "" || name_ == "" || password_ == "") {
        SignInUp(sInput);
        return;
      }

      //Create list of all online players
      std::list<std::string> lk;
      for(const auto &it : *ptr)
        lk.push_back(it.second->GetID());

      //Call main play-loop
      std::cout<<"Befor play: sInput: "<< sInput << " calling with id: " 
        << id_ <<std::endl;
      std::string sOutput = game->play(sInput, id_, lk);

      //Check if game is ended -> close game
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

/**
 * Function load a (txt-)file from disc.
 * @param[in] path (path to file to load)
 * @return return file as string
 */
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

/**
 * Function to load an image from disc.
 * @param[in] path (to images)
 * @param[in] iamge (name of image to load)
 * @return Return image as string.
 */
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


/**
 * main loop
 */
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
