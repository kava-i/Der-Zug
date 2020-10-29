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

  //Tries registering a user.
  REQUIRE(user_manager.DoRegistration("test_manager", "pw", 
        "pw") == "Strength insufficient!");
  REQUIRE(user_manager.DoRegistration("test_manager", "password1234", 
        "password1235") == "Passwords do not match!");
  REQUIRE(user_manager.DoRegistration("test_manager", "password1234", 
        "password1234") == "");
  REQUIRE(user_manager.DoRegistration("test_manager", "password1234", 
        "password1234") == "Username already exists!");

  //Tries to get a user.
  REQUIRE(user_manager.GetUser("humbug") == nullptr);
  REQUIRE(user_manager.GetUser("test_manager") != nullptr);

  //Reloads manager and sees whether test-user was found.
  UserManager user_manager2("../../data/users/", {"attacks", 
    "defaultDialogs", "dialogs", "players", "rooms", "characters", 
    "defaultDescriptions", "details", "items", "quests", "texts"});
  REQUIRE(user_manager.GetUser("test_manager") != nullptr);

  //Delete users.
  user_manager2.DeleteUser("test_manager");
  REQUIRE(user_manager2.GetUser("test_manager") == nullptr);
}

TEST_CASE ("Login and Logout user", "[logging]") {
  //Create user manager.
  UserManager user_manager("../../data/users/", {"attacks", 
    "defaultDialogs", "dialogs", "players", "rooms", "characters", 
    "defaultDescriptions", "details", "items", "quests", "texts"});

  //Prior created user still exists.
  REQUIRE(user_manager.GetUser("test_manager") != nullptr);

  //Try to log random user in
  REQUIRE(user_manager.DoLogin("humbug", "1312") == "Username does not exist.");
  REQUIRE(user_manager.DoLogin("test_manager", "1312") == "Incorrect password.");

  //Try to log existing user in and "fake" cookie creation.
  REQUIRE(user_manager.DoLogin("test_manager", "password1234") == "");
  std::string cookie = "SESSID=" + user_manager.GenerateCookie("test_manager");

  REQUIRE(user_manager.GetUserFromCookie(cookie.c_str()) == "test_manager");
}
