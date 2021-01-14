/**
 * @author georgbuechner
 */

#include <iostream>

#include <catch2/catch.hpp>

#include "user.h"
#include "user_manager.h"
#include "users/inja.hpp"
#include "util//error_codes.h"
#include "util/func.h"

namespace fs = std::filesystem;

TEST_CASE ("Loading pages from user works", "[user_pages]") {
  
  //If already exists, delete test user.
  std::string path_to_test_user = "../../data/users/test";
  std::string path_to_test_user2 = "../../data/users/test2";
  try {
    std::filesystem::remove_all (path_to_test_user);
    std::filesystem::remove_all (path_to_test_user2);
  }
  catch (...) {
    std::cout << path_to_test_user << " not found!" << std::endl;
  }
  
  //Create user manager.
  UserManager user_manager("../../data/users", {"attacks", 
    "defaultDialogs", "dialogs", "players", "rooms", "characters", 
    "defaultDescriptions", "details", "items", "quests", "texts", "images"});

  //Create new user
  user_manager.DoRegistration("test", "password1234", "password1234");
  User* user = user_manager.GetUser("test");
  REQUIRE(user != nullptr);
  //Check that folders for worlds and backups where created
  REQUIRE(func::demo_exists("../../data/users/test/files") == true);
  REQUIRE(func::demo_exists("../../data/users/test/backups") == true);

  //Create empty world.
  std::string world = "Test_World";
  std::string full_path = "../../data/users";
  std::string path = "/test/files/" + world;
  REQUIRE(user->CreateNewWorld(world, 9999) == ErrorCodes::SUCCESS); 
  nlohmann::json players_json;
  REQUIRE(func::LoadJsonFromDisc(full_path+path+"/players/players.json", 
        players_json) == true);
  REQUIRE(players_json.size() != 0);
  REQUIRE(user->CreateNewWorld(world, 9999) == ErrorCodes::ALREADY_EXISTS); 
  REQUIRE(user->CreateNewWorld("../Test_World", 9999) == ErrorCodes::WRONG_FORMAT); 
  REQUIRE(func::demo_exists(full_path + path + "/config.json"));
  REQUIRE(func::demo_exists(full_path + path + "/rooms/test.json"));
  REQUIRE(func::demo_exists(full_path + path + "/players/players.json"));
  REQUIRE(func::demo_exists(full_path + path + "/images/background.jpg"));

  REQUIRE(user->DeleteWorld("humbug") == ErrorCodes::NO_WORLD);
  REQUIRE(user->DeleteWorld(world) == ErrorCodes::SUCCESS);
  REQUIRE(user->CreateNewWorld(world, 9999) == ErrorCodes::SUCCESS); 
  
  //Check that user got a port
  REQUIRE(user->GetPortOfWorld("Test_World") == 9999);

  //Check that newly build world is starting and basic command are running.
  std::string command = "./../../textadventure/build/bin/testing.o"
    " --path ../../data/users/test/files/Test_World/"
    " -p _admin"
    " > ../../data/users/test/logs/Test_World.txt";
  REQUIRE(system(command.c_str()) == 0);

  //GetOverview: Check if world is found on overview page.
  REQUIRE(user->GetOverview(user_manager.GetSharedWorlds("test"), 
        user_manager.GetAllWorlds("test")).find(world) != std::string::npos);
  
  
  //Check that world is still starting and basic command are running.
  REQUIRE(system(command.c_str()) == 0);

  //Add a new file in every category and check that game is still running.
  //Check that accessing objects inside subcategory works
  std::string room_path = path + "/rooms/test_house";


  //Test Creating backups
  REQUIRE(user->CreateBackup("test", "Test_World") == ErrorCodes::SUCCESS);
  //Get backup from folder
  std::string backup = "";
  for (auto p : fs::directory_iterator(full_path+"/test/backups/")) {
    backup = p.path();
    if (backup.find("Test_World") != std::string::npos) break;
  }
  backup = backup.substr(backup.rfind("/")+1);
  //Test if backup can be found on backup page
  REQUIRE(user->GetBackups("test", "Test_World").find(backup) != std::string::npos);

  //Modify objects and test if this works
  nlohmann::json test_room_fail;
  REQUIRE(func::LoadJsonFromDisc("../../data/default_jsons/test_room_fail.json", 
        test_room_fail) == true);
  nlohmann::json write_room_bad;
  write_room_bad["path"] = path+"/rooms/test_house/test_room";
  write_room_bad["json"] = test_room_fail;
  REQUIRE(user->WriteObject(write_room_bad.dump()) == ErrorCodes::GAME_NOT_RUNNING);
  //Test that game is still running
  REQUIRE(system(command.c_str()) == 0);
  //Force to write corrupter json
  write_room_bad["force"] = true;
  REQUIRE(user->WriteObject(write_room_bad.dump()) == ErrorCodes::SUCCESS);
  //Double check by testing whether game now does not start
  REQUIRE(system(command.c_str()) != 0);
  //Restore backup
  REQUIRE(user->RestoreBackup("test", backup) == ErrorCodes::SUCCESS);
  //No test, that game is running again.
  REQUIRE(system(command.c_str()) == 0);

  REQUIRE(user->DeleteBackup("test", backup) == ErrorCodes::SUCCESS);
  //Check that backup is acctually deleted.
  REQUIRE(func::demo_exists(full_path+"/test/backups/"+backup) == false);
  REQUIRE(user->GetBackups("test", "Test_World").find(backup) == std::string::npos);
  //Test updating a file
  nlohmann::json test_room;
  REQUIRE(func::LoadJsonFromDisc("../../data/default_jsons/test_room.json", 
        test_room) == true);
  nlohmann::json write_room_good;
  write_room_good["path"] = path+"/rooms/test_house/test_room";
  write_room_good["json"] = test_room;
  REQUIRE(user->WriteObject(write_room_good.dump()) == ErrorCodes::SUCCESS);
  //Check that game is still running
  REQUIRE(system(command.c_str()) == 0);

  //*** Test accesssing and granting access *** //

  //create new user
  user_manager.DoRegistration("test2", "password1234", "password1234");
  User* user2 = user_manager.GetUser("test2");
  REQUIRE(user2 != nullptr);

  //Do access-tests for user: "test"
  REQUIRE(user->CheckAccessToLocations("/test/files/Test_World") == true);
  REQUIRE(user->CheckAccessToLocations("/test/files/Test_World/attacks") == true);
  REQUIRE(user->CheckAccessToLocations("/test/backups/Test_World/") == true);


  //Do access-tests for user: "test2"
  REQUIRE(user2->CheckAccessToLocations("/test/files/Test_World") == false);
  REQUIRE(user2->CheckAccessToLocations("/test/files/Test_World/attacks") == false);
  REQUIRE(user2->CheckAccessToLocations("/test/backups/Test_World/") == false);

  REQUIRE(user->AddRequest("test2", "Test_World") == true);

  REQUIRE(user_manager.GrantAccessTo("test", "test3", "Test_World") == ErrorCodes::NO_USER);
  REQUIRE(user_manager.GrantAccessTo("test", "test2", "World") == ErrorCodes::NO_WORLD);
  REQUIRE(user_manager.GrantAccessTo("test2", "test2", "Test_World") == ErrorCodes::NO_WORLD);
  REQUIRE(user_manager.GrantAccessTo("test", "test2", "Test_World") == ErrorCodes::SUCCESS);
  
  //Do access-tests for user: "test2" after granting access
  REQUIRE(user2->CheckAccessToLocations("/test/files/Test_World") == true);
  REQUIRE(user2->CheckAccessToLocations("/test/files/Test_World/attacks") == true);
  REQUIRE(user2->CheckAccessToLocations("/test/backups/Test_World/") == true);
  REQUIRE(user2->CreateBackup("test", "Test_World") == ErrorCodes::SUCCESS);
  REQUIRE(user2->RestoreBackup("test", "XXBACKUPYY") == ErrorCodes::ACCESS_DENIED);
  REQUIRE(user2->DeleteBackup("test", "XXBACKUPYY") == ErrorCodes::ACCESS_DENIED);

  //Double check that all test-users are deleted
  try {
    std::filesystem::remove_all (path_to_test_user);
    std::filesystem::remove_all (path_to_test_user2);
  }
  catch (...) {
    std::cout << path_to_test_user << " not found!" << std::endl;
  }
}
