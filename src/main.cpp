#include <iostream>
#define CATCH_CONFIG_RUNNER
#include "Catch2/single_include/catch2/catch.hpp"

#include "CGame.hpp"
#include "JanGeschenk/Webconsole.hpp"
#include "JanGeschenk/Webgame.hpp"
#include "SortedContext.hpp"


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
		    _cout->write("\nName: ");
		    _cout->flush();
		    return;
		}
		_cout->write("\nPassword: ");
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
		    _cout->write(color::red, "Invalid Login please try again!",color::white, "\n\nName: ");
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
		_cout->write("Thanks for playing\n");
		return;
	    }
	    else if(sInput == "change character")
	    {
	        _cout->write("Who do you want to play? (Anna, Jan)");
		return;
	    }

	    std::list<std::string> lk;
	    for(const auto &it : *ptr)
	    {
		lk.push_back(it.second->GetID());
	    }

	    std::cout<<"Got input: "<<sInput<<"; With player id: "<<_id<<std::endl;
	    std::string sOutput = game->play(sInput, _id,lk);
	    std::cout<<"Received Output: "<<sOutput<<std::endl;
	    _cout->write(sOutput);
	    _cout->flush();
	}
};
#ifdef _COMPILE_UNIT_TEST_
int main(int argc, char **argv)
{
    int result = Catch::Session().run( argc, argv );
    if(result!=0)
    {
	std::cout<<"Some tests failed can not proceed with the programm!"<<std::endl;
	return result;
    }
    return result;
}
#else
int main(int, char **)
{
    CGame currentGame;
    game = &currentGame;
    Webgame<WebserverGame> gl;
    gl.run();
}
#endif
