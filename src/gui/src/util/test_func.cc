/**
 * @author georgbuechner
 */

#include <iostream>
#include <string>
#include <vector>

#include <catch2/catch.hpp>

#include "func.h"

TEST_CASE ("Return to lower", "[to_lower]") {
  SECTION ("Handle normal use-cases") {
    std::string str = "HELLO WORLD";
    REQUIRE(func::ReturnToLower(str) == "hello world");
    str = "HELLO 1234 WORLD";
    REQUIRE(func::ReturnToLower(str) == "hello 1234 world");
    str = "HELLO §$%&/()= WORLD";
    REQUIRE(func::ReturnToLower(str) == "hello §$%&/()= world");
  }
  SECTION ("Handle german specific cases") {
    std::string str = "HÄLLO WÖRLD";
    REQUIRE(func::ReturnToLower(str) == "hällo wörld");
  }
}

TEST_CASE ("Split works as expected", "[split]") {
  std::vector<std::string> vec = func::Split("Hello;hello", ";");
  REQUIRE(vec[0] == "Hello");
  REQUIRE(vec[1] == "hello");

  vec = func::Split("Hello;hello;", ";");
  REQUIRE(vec[0] == "Hello");
  REQUIRE(vec[1] == "hello");
  REQUIRE(vec[2] == "");
}

TEST_CASE ("Loading pages from disc is working", "[get_page]") {
  REQUIRE(func::GetPage("humbug") == "");
  std::string read_main = func::GetPage("web/main.html");
  REQUIRE(read_main.find("KAVA-I Txt Game*Creator") != std::string::npos);
}


TEST_CASE ("Loading images from disc is working", "[get_page]") {
  REQUIRE(func::GetImage("humbug") == "");
  REQUIRE(func::GetImage("web/images/background.jpg").length() > 0);
}

