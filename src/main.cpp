#include <iostream>
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include "CGame.hpp"
#include "JanGeschenk/Webconsole.hpp"
#include "JanGeschenk/Webgame.hpp"
#include "SortedContext.hpp"
#include "Webcmd.hpp"


CGame *game;

class WebserverGame
{
    private:
	Webconsole *_cout;
	std::string _name;
	std::string _password;
	std::string _id;
    public:
	WebserverGame(Webconsole *cout)
	{
        std::cout << "Starting....\n";

	    _name = "";
	    _password = "";
	    _id = "";
	    _cout = cout;
	    _cout->write("Name: ");
	    _cout->flush();
	}

	const std::string &GetName()
	{
	    return _name;
	}

	const std::string &GetID()
	{
	    return _id;
	}

	void onmessage(std::string sInput,std::map<decltype(websocketpp::lib::weak_ptr<void>().lock().get()),WebserverGame*> *ptr)
	{
	    if(_name=="")
	    {
		_name=sInput;
		if(_name=="")
		{
		    _cout->write(Webcmd::set_color(Webcmd::color::RED),"\nName: ");
		    _cout->flush();
		    return;
		}
		_cout->write(Webcmd::set_color(Webcmd::color::ORANGE),"\nPassword: ");
		_cout->flush();
		return;
	    }

	    if(_password=="")
	    {
		_password = sInput;
		if(_password=="")
		{
		    _cout->write("\nPassword: ");
		    _cout->flush();
		    return;
		}
		_id = game->checkLogin(_name,_password);
		if(_id=="")
		{
		    _name = "";
		    _password = "";
		    _cout->write(Webcmd::set_color(Webcmd::color::RED), "Invalid Login please try again!",color::white, "\n\nName: ");
		    _cout->flush();
		    return;
		}
		sInput = game->startGame(sInput,_id, _cout);
		_cout->write(sInput);
		_cout->flush();
		return;
	    } 



	    if(sInput == ":q")
	    {
		_cout->write(Webcmd::set_sound("sounds/fight.mp3"),Webcmd::set_color(Webcmd::color::GREEN),"Thanks for playing\n");
		_cout->flush();
		return;
	    }
	    else if(sInput == "change character")
	    {
		_cout->write("Who do you want to play? (Anna, Jan)");
		_cout->flush();
		return;
	    }

	    std::list<std::string> lk;
	    for(const auto &it : *ptr)
	    {
		lk.push_back(it.second->GetID());
	    }

	    std::cout<<"Befor play: sInput: "<<sInput<<" calling with id: "<<_id<<std::endl;
	    std::string sOutput = game->play(sInput, _id,lk);
	    std::cout<<"Got output: "<<sOutput<<std::endl;
	    std::cout<<"Writing to client now"<<std::endl;
	    _cout->write(sOutput);
	    std::cout<<"Send the output to client"<<std::endl;
	    _cout->flush();
	}
};

int main(int x, char **argc)
{
    if(x>=2)
    {
	std::string wt = argc[1];
	if(wt=="-d")
	{
	    int result = Catch::Session().run( x, argc );
	    if(result!=0)
	    {
		std::cout<<"Some tests failed can not proceed with the programm!"<<std::endl;
		return result;
	    }
	    return 0;
	}
    }

    CGame currentGame;
    game = &currentGame;
    Webgame<WebserverGame> gl;
    gl.run();
}

