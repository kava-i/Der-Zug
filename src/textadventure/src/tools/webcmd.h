#pragma once

#include <iostream>
#include <string>

namespace Webcmd
{
    enum color
    {
    BLACK = 0,
	WHITE = 1,
	RED = 2,
	GREEN = 3,
	YELLOW = 4,
	BLUE = 5,
	ORANGE = 6,
    DARK = 8,
    WHITEDARK = 9
    };

    std::string set_color(color cl);

    std::string set_sound(std::string filename);
};

