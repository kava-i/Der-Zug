/**
 * @author georgbuechner
 */

#include <iostream>

#include <catch2/catch.hpp>

#include "util/func.h"
#include "user.h"
#include "user_manager.h"

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
  std::string full_path = "../../data/users/";
  std::string path = "test_manager/files/" + world;
  REQUIRE(user->CreateNewWorld(world) == ""); 
  REQUIRE(user->CreateNewWorld(world) == "World already exists."); 
  REQUIRE(user->CreateNewWorld("../Test_World") == "Wrong format."); 
  REQUIRE(func::demo_exists(full_path + path + "/config.json"));
  REQUIRE(func::demo_exists(full_path + path + "/rooms/test.json"));
  REQUIRE(func::demo_exists(full_path + path + "/players/players.json"));

  //Check that newly build world is starting and basic command are running.
  std::string command = "./../../textadventure/build/bin/testing.o "
    "--path ../../data/users/test_manager/files/Test_World/ -p test";
  REQUIRE(system(command.c_str()) == 0);

  //GetOverview: Check if world is found on overview page.
  REQUIRE(user->GetOverview().find(world) != std::string::npos);
  
  //GetWorld: Check if basic categories are found and func react to wrong path.
  REQUIRE(user->GetWorld("humbug", "humbug") == "");
  REQUIRE(user->GetWorld(path, world).find("attacks") != std::string::npos);
 
  //GetCategory: Check if page is returned, and func reacts to wrong path.
  REQUIRE(user->GetCategory("hum/bug", "hum", "bug") == "");
  REQUIRE(user->GetCategory(path+"/rooms", world, "rooms") != "");

  //Check adding files is working.
  REQUIRE(user->AddFile("hum/bug", "humbug") == "Path not found.");
  REQUIRE(user->AddFile(path+"/rooms", "../test_house") == "Wrong format.");
  REQUIRE(user->AddFile(path+"/rooms", "test_house") == "");
  REQUIRE(user->AddFile(path+"/rooms", "test_house") == "File already exists.");
  REQUIRE(func::demo_exists(full_path + path + "/rooms/test_house.json") 
      == true);

  //Check that world is still starting and basic command are running.
  REQUIRE(system(command.c_str()) == 0);

  //Add a new file in every category and check that game is still running.
  REQUIRE(user->AddFile(path+"/attacks", "test_attacks") == "");
  REQUIRE(user->AddFile(path+"/defaultDialogs", "test_default_dialogs") == "");
  REQUIRE(user->AddFile(path+"/dialogs", "test_dialogs") == "");
  REQUIRE(user->AddFile(path+"/characters", "test_characters") == "");
  REQUIRE(user->AddFile(path+"/defaultDescriptions", "test_default_descs") 
      == "");
  REQUIRE(user->AddFile(path+"/items", "test_items") == "");
  REQUIRE(user->AddFile(path+"/details", "test_details") == "");
  REQUIRE(user->AddFile(path+"/quests", "test_quests") == "");
  REQUIRE(user->AddFile(path+"/texts", "test_texts") == "");
  REQUIRE(system(command.c_str()) == 0);
  REQUIRE(user->AddFile(path+"/players", "test_house") == 
      "Not supported category.");

  //Check that newly added subcategories are be found in category
  REQUIRE(user->GetCategory(path+"/rooms", world, "rooms").find("test_house") 
      != std::string::npos);

  //Check that accessing objects inside subcategory works
  std::string room_path = path + "/rooms/test_house";
  REQUIRE(user->GetObjects("hum/bug/quatsch", "hum", "bug", "quatsch") == "");
  REQUIRE(user->GetObjects(room_path, world, "rooms", "test_house") != "");

  //Check that accessing object is working
  REQUIRE(user->GetObjects("hum/bug/quatsch", "hum", "bug", "quatsch") == "");
  REQUIRE(user->GetObjects(room_path, world, "rooms", "test_house") != "");

  //Check that adding a new empty room is working
  REQUIRE(user->AddNewObject(room_path, "test_room") == "");
  REQUIRE(system(command.c_str()) == 0);
  REQUIRE(user->AddNewObject(path+"/attacks/test_attacks","test_attack") == "");
  REQUIRE(user->AddNewObject(path+"/dialogs/test_dialogs","test_dialog") == "");
  REQUIRE(user->AddNewObject(path+"/characters/test_characters", 
        "test_character") == "");
  REQUIRE(system(command.c_str()) == 0);
  REQUIRE(user->AddNewObject(path+"/items/test_items", "test_item") == "");
  REQUIRE(user->AddNewObject(path+"/details/test_details","test_details")=="");
  REQUIRE(user->AddNewObject(path+"/quests/test_quests", "test_quest") == "");
  REQUIRE(user->AddNewObject(path+"/texts/test_texts", "test_text") == "");
  REQUIRE(system(command.c_str()) == 0);
  REQUIRE(user->AddNewObject(path+"/defaultDescriptions/test_default_descs",
        "test_desc") == "");
  REQUIRE(system(command.c_str()) == 0);
  REQUIRE(user->AddNewObject(path+"/defaultDialogs/test_default_dialogs",
        "test_dialog") == "");
  REQUIRE(system(command.c_str()) == 0);

  //Check if newly create objects can be found on html page
  REQUIRE(user->GetObjects(room_path, world, "rooms", "test_house")
      .find("test_room") != std::string::npos);

  //Check if object pages can be reached
  REQUIRE(user->GetObject(path+"/rooms/test_house/test_room/", world, "rooms", 
        "test_house","test_room") != "Object can't be viewed yet.");
  REQUIRE(user->GetObject(path+"/attacks/test_attacks/test_attack/", world, "attacks", 
        "test_attacks","test_attack") != "Object can't be viewed yet.");
  REQUIRE(user->GetObject(path+"/dialogs/test_dialogs/test_dialog/", world "dialogs", 
        "test_dialogs","test_dialog") != "Object can't be viewed yet.");
  REQUIRE(user->GetObject(path+"/characters/test_characters/test_character/", world, 
        "characters", "test_characters","test_character") != "Object can't be viewed yet.");
  REQUIRE(user->GetObject(path+"/items/test_items/test_item/", world, "items", 
        "test_items","test_item") != "Object can't be viewed yet.");
  REQUIRE(user->GetObject(path+"/details/test_details/test_detail/", world, "details", 
        "test_details","test_detail") != "Object can't be viewed yet.");
  REQUIRE(user->GetObject(path+"/details/test_details/test_detail/", world, "details", 
        "test_details","test_detail") != "Object can't be viewed yet.");
  REQUIRE(user->GetObject(path+"/quests/test_quests/test_quest/", world, "quests", 
          "test_quests","test_quest") != "Object can't be viewed yet.");
  REQUIRE(user->GetObject(path+"/texts/test_texts/test_text/", world, "details", 
          "test_texts","test_text") != "Object can't be viewed yet.");
  REQUIRE(user->GetObject(path+"/defaultDescriptions/test_default_descs/test_desc/", 
        world, "defaultDescriptions", "test_default_descs","test_desc") 
      != "Object can't be viewed yet.");
  REQUIRE(user->GetObject(path+"/defaultDialogs/test_default_dialogs/test_dialog/", 
        world, "defaultDialogs", "test_default_dialogs","test_dialog") 
      != "Object can't be viewed yet.");
}
