/**
 * @author georgbuechner
 */

#include <filesystem>
#include <iostream>

#include <catch2/catch.hpp>

#include "user_manager.h"

TEST_CASE ("Registering, loading and removing users works", "[creating]") {
  //If already exists, delete test user.
  std::string path_to_test_user = "../../data/users/test_manager";
  try {
    std::filesystem::remove_all (path_to_test_user);
  }
  catch (...) {
    std::cout << path_to_test_user << " not found!" << std::endl;
  }
  
  //Create user manager.
  UserManager user_manager("../../data/users/", {"attacks", 
    "defaultDialogs", "dialogs", "players", "rooms", "characters", 
    "defaultDescriptions", "details", "items", "quests", "texts"});

  SECTION ("Registering user works") {
        REQUIRE(user_manager.DoRegistration("test_manager", "pw", 
          "pw") == "Strength insufficient!");
    REQUIRE(user_manager.DoRegistration("test_manager", "password1234", 
          "password1235") == "Passwords do not match!");
    REQUIRE(user_manager.DoRegistration("test_manager", "password1234", 
          "password1234") == "");
    REQUIRE(user_manager.DoRegistration("test_manager", "password1234", 
          "password1234") == "Username already exists!");
  }
}
