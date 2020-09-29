#include <iostream>
#include <string>

#include "logic_parser.h"

int main() {
  LogicParser parser({
      {"inventory","raincoat;red wine;ticket"},
      {"room","trainstation_fernzüge"},
      {"cmd","go"},
      {"input","zu gleis 3"},
      {"logic", "5"},
      {"wahrnehmung", "4"}});

  std::cout << "---- Logic parser ----" << std::endl;

  std::vector<std::string> inputs = {
    //"(a=b) | ((a=a) & (a=a))",
    "(input~zu Gleis 3) & (room=trainstation_fernzüge) & (inventory:ticket) "};
    //"((a=a) | (a=a)) & (a=a)"};

  for (auto input : inputs) {
    bool x = parser.Success(input);
    std::cout << "evaluates to: " << x << std::endl;
  }
  return 0;
}


