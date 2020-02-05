#pragma once

#include <iostream>
#include <string>

namespace Webcmd
{
    enum color
    {
	WHITE = 1,
	RED = 2,
	GREEN = 3,
	YELLOW,
	BLUE,
	ORANGE
    };

    std::string set_color(color cl);
/*
    {
	char ch = cl;
	std::string x;
	x.push_back(ch);
	return x;
    }*/

    std::string set_sound(std::string filename);
    /*
    {
	char ch = 7;
	std::string ret;
	ret.push_back(ch);
	ret+=filename;
	ret.push_back(ch);
	return ret;
    }
*/
};

