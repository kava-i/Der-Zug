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


TEST_CASE ("Loading images from disc is working", "[get_image]") {
  REQUIRE(func::GetImage("humbug") == "");
  REQUIRE(func::GetImage("web/images/background.jpg").length() > 0);
}

TEST_CASE ("Checking if file exists is working", "[file_exists]") {
  REQUIRE(func::demo_exists("humbug") == false);
  REQUIRE(func::demo_exists("web/images/background.jpg") == true);
}

TEST_CASE("Checking if TimeSinceEpoch returns a value", "[time]") {
  REQUIRE(func::TimeSinceEpoch() > 0);
}

TEST_CASE("hashing returns a value, leaves passed string unchanged", "[hass]") {
  std::string str = "mypassword12§ß";
  std::string check_str = str;
  REQUIRE(func::hash_sha3_512(str).length() > 0);
  REQUIRE(str == check_str);
}

