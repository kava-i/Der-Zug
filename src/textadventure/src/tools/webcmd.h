#pragma once

#include <iostream>
#include <map>
#include <string>

namespace Webcmd {
  enum color {
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
  std::string set_image(std::string filename);

	const std::map<std::string, std::string> COLOR_MAPPING = {
		{"black", set_color(BLACK)},
    {"white", set_color(WHITE)},
    {"red", set_color(RED)},
    {"green", set_color(GREEN)},
    {"yellow", set_color(YELLOW)},
    {"blue", set_color(BLUE)},
    {"orange", set_color(ORANGE)},
    {"dark", set_color(DARK)},
    {"whitedark", set_color(WHITEDARK)}
	};
};

