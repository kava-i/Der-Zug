#ifndef SRC_WEBGAME_H_
#define SRC_WEBGAME_H_

#include "JanGeschenk/Webconsole.hpp"
#include "JanGeschenk/Webgame.hpp"
#include "game/game.h"
#include "eventmanager/sorted_context.h"
#include "tools/webcmd.h"
#include <memory>

class WebserverGame {
  private:
    std::string name_;
    std::string password_;
    std::string id_;
    std::string char_name_;
    std::string sign_in_up_;
		bool game_started_;
    Webconsole* cout_;
    std::shared_ptr<CGame> game_;

  public:
    WebserverGame(Webconsole *cout, std::shared_ptr<CGame> game) {
      std::cout << "Starting....\n";
      name_ = "";
      password_ = "";
      id_ = "";
      char_name_ = "";
      sign_in_up_ = "";
      cout_ = cout;
      game_ = game;
			game_started_ = false;
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

    void Login(std::string in) {
      std::cout << "On to getting credentials" << std::endl;
      if (name_ == "") {
        name_ = in ;
        send(" " + in + "\n");
        cout_->write(Webcmd::set_color(Webcmd::color::ORANGE), "Password: ");
        cout_->flush();
        return;
      }

			if (password_ == "") {
        password_ = in;
        std::string str(in.length(), '*');
        send(" " + str + "\n");
				DoLogin();
  		}
			else if (char_name_ == "") {
        char_name_ = in;
        send(" " + char_name_ + "\n");
				DoLogin();
			}
    }

		void DoLogin() {
        std::string out = game_->checkLogin(name_, password_, sign_in_up_ == "l", id_, char_name_);
        send(out);
        if (id_ == "" && out.find("> Pick character") == std::string::npos) {
          password_ = "";
          name_ = "";
          char_name_ = "";
        }
        else if (id_ != "") {
					game_started_ = true;
          std::string output = game_->startGame(id_, cout_);
          std::cout << "Got output: " << output << std::endl;
          send(output);
        }
		}

    void SignInUp(std::string in) {
      if (sign_in_up_ == "") {
        std::cout << "Choosing login/ register";
        if (in == "l" || in == "r" || in == "login" || in == "register") {
          sign_in_up_ = in.substr(0,1);
          cout_->write(Webcmd::set_color(Webcmd::color::ORANGE), "\nid: ");
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
      else
        Login(in);
    }

    void onmessage(std::string input, std::map<decltype(websocketpp::lib::
          weak_ptr<void>().lock().get()), WebserverGame*> *ptr, bool& global_shutdown) {

      //Check for login/register-phase
      if (!game_started_) {
        SignInUp(input);
        return;
      }
      func::convertToLower(input);

      //Create list of all online players
      std::list<std::string> lk;
      for(const auto &it : *ptr)
        lk.push_back(it.second->GetID());

      //Call main play-loop
      std::cout << "Befor play: input: " << input << " calling with id: " << id_ << std::endl;
      std::string sOutput = game_->play(input, id_, lk);

      //Check if game is ended -> close game
      send(sOutput);
      std::cout << "Send the output to client: " << sOutput <<std::endl;
    }
};

#endif
