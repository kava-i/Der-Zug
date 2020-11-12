/**
 * @author georgbuechner
 */
#include "util/func.h"
#include "nlohmann/json.hpp"
#include "util/error_codes.h"
#include <string>
#define CATCH_CONFIG_MAIN

#include <iostream>
#include <thread>

#include <catch2/catch.hpp>
#include <httplib.h>

#include "server_frame.h"
#include "util/func.h"

namespace fs=std::filesystem;

void del_test_user(std::string username) {
  //If already exists, delete test data.
  try{
    fs::remove_all("../../data/users/" + username);
    std::cout << username << " deleted." << std::endl;
  }
  catch(...) {return; }
}

TEST_CASE("Server is working as expected", "[server]") {
  del_test_user("test");
  del_test_user("test2");
 
  ServerFrame server;

  std::thread t1([&server]() {
    server.Start(4444);
  });

  std::thread t2([&server]() {
        while (!server.IsRunning()) {
          std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        httplib::Client cl("localhost", 4444);
        cl.set_connection_timeout(2);

        SECTION("Get-request serving files works!") {
          //Non-existing page should return status 404 not found.
          auto resp = cl.Get("/humbug");
          REQUIRE(resp->status == 404);

          //Existing page should return status 200 and content should be set.
          resp = cl.Get("/");
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body.length() > 0);
        }

        SECTION("Registering, Deleting and accessing user works.") {
          //Check for correkt response when sending registration-request.
          //try to create test-user (fail)
          nlohmann::json request;
          request["id"] = "test";
          request["pw1"] = "password";
          request["pw2"] = "password0408";
          auto resp = cl.Post("/api/user_registration", {}, request.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 401);
          request["pw1"] = "password0408";

          //Create test user (success)
          resp = cl.Post("/api/user_registration", {}, request.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);

          //Check if cookie has been sent, extract cookie and add to headers.
          REQUIRE(resp->get_header_value("Set-Cookie").length() > 32);
          std::string cookie = resp->get_header_value("Set-Cookie");
          cookie = cookie.substr(0, cookie.find(";"));
          httplib::Headers headers_1 = { { "Cookie", cookie } };

          resp = cl.Get("/overview", headers_1);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");

          //Test deleting user
          resp = cl.Post("/api/user_delete", headers_1, "",
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          REQUIRE(func::demo_exists("../../data/users/test") == false);
          resp = cl.Get("/overview", headers_1);
          REQUIRE(resp->status == 302);
        }

        SECTION("Loging in and out is working") {
          //Register new test user
          nlohmann::json request;
          request["id"] = "test";
          request["pw1"] = "password0408";
          request["pw2"] = "password0408";
          auto resp = cl.Post("/api/user_registration", {}, request.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          
          //Check if cookie has been sent, extract cookie and add to headers.
          REQUIRE(resp->get_header_value("Set-Cookie").length() > 32);
          std::string cookie = resp->get_header_value("Set-Cookie");
          cookie = cookie.substr(0, cookie.find(";"));
          httplib::Headers headers_1 = { { "Cookie", cookie } };
          //Check that overview page can be accessed
          resp = cl.Get("/overview", headers_1);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");

          resp = cl.Post("/api/user_logout", headers_1, request.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);

          //Now, overview page should not be accessable anymore
          resp = cl.Get("/overview", headers_1);
          REQUIRE(resp->status == 302);

          //Log user in.
          request["username"] = "test";
          request["password"] = "password0408";
          resp = cl.Post("/api/user_login", {}, request.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          
          //Check if cookie has been sent, extract cookie and add to headers.
          REQUIRE(resp->get_header_value("Set-Cookie").length() > 32);
          cookie = resp->get_header_value("Set-Cookie");
          cookie = cookie.substr(0, cookie.find(";"));
          headers_1 = { { "Cookie", cookie } };
          resp = cl.Get("/overview", headers_1);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");
        }

        SECTION("Accessing and creating files works") {
          //Register new test user
          nlohmann::json request;
          request["id"] = "test";
          request["pw1"] = "password0408";
          request["pw2"] = "password0408";
          auto resp = cl.Post("/api/user_registration", {}, request.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          
          //Check if cookie has been sent, extract cookie and add to headers.
          REQUIRE(resp->get_header_value("Set-Cookie").length() > 32);
          std::string cookie = resp->get_header_value("Set-Cookie");
          cookie = cookie.substr(0, cookie.find(";"));
          httplib::Headers headers_1 = { { "Cookie", cookie } };
          //Check that overview page can be accessed
          resp = cl.Get("/overview", headers_1);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");

          //Create a new world
          nlohmann::json new_world;
          new_world["world"] = "new_world";
          resp = cl.Post("/api/add_world", headers_1, new_world.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          resp = cl.Get("/overview", headers_1);
          REQUIRE(resp->body.find("new_world") != std::string::npos);
          
          //Check that newly build world is starting and basic command are running.
          std::string command = "./../../textadventure/build/bin/testing.o"
              " --path ../../data/users/test/files/new_world/"
              " -p test"
              " > ../../data/users/test/logs/new_world.txt";
          REQUIRE(system(command.c_str()) == 0);

          //Check accessing categories-page
          resp = cl.Get("/test/files/new_world", headers_1);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body.find("config") != std::string::npos);
          REQUIRE(resp->body.find("attacks") != std::string::npos);

          //Check accessing categories
          resp = cl.Get("/test/files/new_world/config", headers_1);
          REQUIRE(resp->status == 200);
          resp = cl.Get("/test/files/new_world/rooms", headers_1);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body.find("test") != std::string::npos);

          //The test-room-file should already exist
          resp = cl.Get("/test/files/new_world/rooms/test", headers_1);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body.find("test_room") != std::string::npos);

          //Add a few new files to categories
          nlohmann::json new_sub;
          new_sub["subcategory"] = "test_attacks";
          new_sub["path"] = "/test/files/new_world/attacks";
          resp = cl.Post("/api/add_subcategory", headers_1, new_sub.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          //test_attacks can be found on page
          resp = cl.Get("/test/files/new_world/attacks", headers_1);
          REQUIRE(resp->body.find("test_attacks") != std::string::npos);
          //test attacks can be found
          resp = cl.Get("/test/files/new_world/attacks/test_attacks", headers_1);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");

          //Check that game is still running.
          REQUIRE(system(command.c_str()) == 0);

          //Add a new object
          nlohmann::json new_obj;
          new_obj["name"] = "test_attack";
          new_obj["path"] = "/test/files/new_world/attacks/test_attacks";
          resp = cl.Post("/api/add_object", headers_1, new_obj.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          //test_attacks can be found on page
          resp = cl.Get("/test/files/new_world/attacks/test_attacks", headers_1);
          REQUIRE(resp->body.find("test_attack") != std::string::npos);
          //test attacks can be found
          resp = cl.Get("/test/files/new_world/attacks/test_attacks/test_attack", 
              headers_1);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");

          //Create backup
          nlohmann::json create_backup;
          create_backup["world"] = "new_world";
          create_backup["user"] = "test";
          resp = cl.Post("/api/create_backup", headers_1, create_backup.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body == std::to_string(ErrorCodes::SUCCESS));
          
          //Modify objects and test if this works
          nlohmann::json test_room_fail;
          REQUIRE(func::LoadJsonFromDisc("../../data/default_jsons/test_room_fail.json", 
                test_room_fail) == true);
          nlohmann::json write_room_bad;
          write_room_bad["path"] = "/test/files/new_world/rooms/test/test_room";
          write_room_bad["json"] = test_room_fail;
          //Write object.
          resp = cl.Post("/api/write_object", headers_1, write_room_bad.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 401);
          REQUIRE(resp->body == std::to_string(ErrorCodes::GAME_NOT_RUNNING));
          write_room_bad["force"] = true;
          resp = cl.Post("/api/write_object", headers_1, write_room_bad.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body == std::to_string(ErrorCodes::SUCCESS));
          //Get backup from folder
          std::string backup = "";
          for (auto p : fs::directory_iterator("../../data/users/test/backups/")) {
            backup = p.path();
            if (backup.find("Test_World") != std::string::npos) break;
          }
          backup = backup.substr(backup.rfind("/"));
          //Restore backup
          nlohmann::json restore_backup;
          restore_backup["user"] = "test";
          restore_backup["backup"] = backup;
          resp = cl.Post("/api/restore_backup", headers_1, restore_backup.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body == std::to_string(ErrorCodes::SUCCESS));
          nlohmann::json test_room;
          REQUIRE(func::LoadJsonFromDisc("../../data/default_jsons/test_room.json", 
                test_room) == true);
          nlohmann::json write_room_good;
          write_room_good["path"] = "/test/files/new_world/rooms/test/test_room";
          write_room_good["json"] = test_room;
          resp = cl.Post("/api/write_object", headers_1, write_room_good.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body == std::to_string(ErrorCodes::SUCCESS));
          //Test deleting backup
          nlohmann::json delete_backup;
          delete_backup["user"] = "test";
          delete_backup["backup"] = backup;
          resp = cl.Post("/api/delete_backup", headers_1, delete_backup.dump(),
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body == std::to_string(ErrorCodes::SUCCESS));

          // *** ACCESS TESTS *** //
          //
          //Register new test user
          nlohmann::json new_user;
          new_user["id"] = "test2";
          new_user["pw1"] = "password0408";
          new_user["pw2"] = "password0408";
          resp = cl.Post("/api/user_registration", {}, new_user.dump(), 
              "application/x-www-form-urlencoded");
          std::cout << "ErrorCode: " << resp->body << std::endl;
          REQUIRE(resp->status == 200);
          
          //Check if cookie has been sent, extract cookie and add to headers.
          REQUIRE(resp->get_header_value("Set-Cookie").length() > 32);
          cookie = resp->get_header_value("Set-Cookie");
          cookie = cookie.substr(0, cookie.find(";"));
          httplib::Headers headers_2 = { { "Cookie", cookie } };
          //Check that overview page can be accessed
          resp = cl.Get("/overview", headers_2);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");

          resp = cl.Get("/test/files/new_world", headers_2);
          REQUIRE(resp->status == 302);
          resp = cl.Post("/api/add_object", headers_2, new_obj.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->body == std::to_string(ErrorCodes::ACCESS_DENIED));
          resp = cl.Post("/api/add_subcategory", headers_2, new_sub.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->body == std::to_string(ErrorCodes::ACCESS_DENIED));

          resp = cl.Post("/api/write_object", headers_2, write_room_good.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->body == std::to_string(ErrorCodes::ACCESS_DENIED));

          //Check that not backup-operations can be done.
          resp = cl.Post("/api/create_backup", headers_2, create_backup.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->body == std::to_string(ErrorCodes::ACCESS_DENIED));
          resp = cl.Post("/api/restore_backup", headers_2, restore_backup.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->body == std::to_string(ErrorCodes::ACCESS_DENIED));
          resp = cl.Post("/api/delete_backup", headers_2, delete_backup.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->body == std::to_string(ErrorCodes::ACCESS_DENIED));
        }
        server.Stop();
    });
  t1.join();
  t2.join();
}
