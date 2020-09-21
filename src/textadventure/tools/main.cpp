#include <iostream>
#include <string>

#include "logic_parser.h"

int main() {
  std::string input = "";
  LogicParser parser;

  std::cout << "---- Logic parser ----" << std::endl;
  std::cout << "> ";
  getline(std::cin, input);

  std::cout << input << " = " << std::endl;
  bool x = parser.Success(input);
  std::cout << x << std::endl;
  return 0;
}


