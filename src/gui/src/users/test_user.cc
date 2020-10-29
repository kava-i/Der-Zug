/**
 * @author georgbuechner
 */

#include <iostream>

#include <catch2/catch.hpp>

#include "user_manager.h"
#include "user.h"

TEST_CASE ("Loading pages from user works", "[user_pages]") {
  
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

  //Create new user
  user_manager.DoRegistration("test_manager", "password1234", "password1234");
  User* user = user_manager.GetUser("test_manager");
  REQUIRE(user != nullptr);

  //Create empty world.
  std::string world = "Test_World";
  REQUIRE(user->CreateNewWorld(world) == ""); 
  REQUIRE(user->CreateNewWorld(world) == "World already exists."); 
  REQUIRE(user->CreateNewWorld("../Test_World") == "Wrong format."); 

  //GetOverview: Check if world is found on overview page.
  REQUIRE(user->GetOverview().find(world) != std::string::npos);
  
  //GetWorld: Check if basic categories are found and func react to wrong path.
  std::string path = "test_manager/files/"+world;
  REQUIRE(user->GetWorld("humbug", "humbug") == "");
  REQUIRE(user->GetWorld(path, world).find("attacks") != std::string::npos);
 
  //GetCategory: Check if page is returned, and func reacts to wrong path.
  path += "/rooms";
  REQUIRE(user->GetCategory("humbug", "humbug") == "");
  REQUIRE(user->GetWorld(path, world, "rooms") != "");
}
