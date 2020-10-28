/**
 * @author georgbuechner
 */

#include <iostream>
#include <string>

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

TEST_CASE ("Loading images from disc is working", "[get_page]") {

}

