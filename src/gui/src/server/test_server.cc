/**
 * @author georgbuechner
 */
#include "util/func.h"
#include "nlohmann/json.hpp"
#include <string>
#define CATCH_CONFIG_MAIN

#include <iostream>
#include <thread>

#include <catch2/catch.hpp>
#include <httplib.h>

#include "server_frame.h"
#include "util/func.h"

void del_test_user(std::string username) {
  //If already exists, delete test data.
  try{
    std::filesystem::remove_all("../../data/users/" + username);
    std::cout << username << " deleted." << std::endl;
  }
  catch(...) {return; }
}

TEST_CASE("Server is working as expected", "[server]") {
  del_test_user("test1");
 
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
          request["id"] = "test1";
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
          httplib::Headers headers = { { "Cookie", cookie } };

          resp = cl.Get("/overview", headers);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");

          //Test deleting user
          resp = cl.Post("/api/user_delete", headers, "",
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          REQUIRE(func::demo_exists("../../data/users/test1") == false);
          resp = cl.Get("/overview", headers);
          REQUIRE(resp->status == 302);
        }

        SECTION("Loging in and out is working") {
          //Register new test user
          nlohmann::json request;
          request["id"] = "test1";
          request["pw1"] = "password0408";
          request["pw2"] = "password0408";
          auto resp = cl.Post("/api/user_registration", {}, request.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          
          //Check if cookie has been sent, extract cookie and add to headers.
          REQUIRE(resp->get_header_value("Set-Cookie").length() > 32);
          std::string cookie = resp->get_header_value("Set-Cookie");
          cookie = cookie.substr(0, cookie.find(";"));
          httplib::Headers headers = { { "Cookie", cookie } };
          //Check that overview page can be accessed
          resp = cl.Get("/overview", headers);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");

          resp = cl.Post("/api/user_logout", headers, request.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);

          //Now, overview page should not be accessable anymore
          resp = cl.Get("/overview", headers);
          REQUIRE(resp->status == 302);

          //Log user in.
          request["username"] = "test1";
          request["password"] = "password0408";
          resp = cl.Post("/api/user_login", {}, request.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          
          //Check if cookie has been sent, extract cookie and add to headers.
          REQUIRE(resp->get_header_value("Set-Cookie").length() > 32);
          cookie = resp->get_header_value("Set-Cookie");
          cookie = cookie.substr(0, cookie.find(";"));
          headers = { { "Cookie", cookie } };
          resp = cl.Get("/overview", headers);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");
        }

        SECTION("Accessing and creating files works") {
          //Register new test user
          nlohmann::json request;
          request["id"] = "test1";
          request["pw1"] = "password0408";
          request["pw2"] = "password0408";
          auto resp = cl.Post("/api/user_registration", {}, request.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          
          //Check if cookie has been sent, extract cookie and add to headers.
          REQUIRE(resp->get_header_value("Set-Cookie").length() > 32);
          std::string cookie = resp->get_header_value("Set-Cookie");
          cookie = cookie.substr(0, cookie.find(";"));
          httplib::Headers headers = { { "Cookie", cookie } };
          //Check that overview page can be accessed
          resp = cl.Get("/overview", headers);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");

          //Create a new world
          nlohmann::json new_world;
          new_world["world"] = "new_world";
          resp = cl.Post("/api/add_world", headers, new_world.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          resp = cl.Get("/overview", headers);
          REQUIRE(resp->body.find("new_world") != std::string::npos);
          
          //Check that newly build world is starting and basic command are running.
          std::string command = "./../../textadventure/build/bin/testing.o"
              " --path ../../data/users/test1/files/new_world/"
              " -p test"
              " > ../../data/users/test1/logs/new_world.txt";
          REQUIRE(system(command.c_str()) == 0);

          //Check accessing categories-page
          resp = cl.Get("/test1/files/new_world", headers);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body.find("config") != std::string::npos);
          REQUIRE(resp->body.find("attacks") != std::string::npos);

          //Check accessing categories
          resp = cl.Get("/test1/files/new_world/config", headers);
          REQUIRE(resp->status == 200);
          resp = cl.Get("/test1/files/new_world/rooms", headers);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body.find("test") != std::string::npos);

          //The test-room-file should already exist
          resp = cl.Get("/test1/files/new_world/rooms/test", headers);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body.find("test_room") != std::string::npos);

          //Add a few new files to categories
          nlohmann::json new_sub;
          new_sub["subcategory"] = "test_attacks";
          new_sub["path"] = "/test1/files/new_world/attacks";
          resp = cl.Post("/api/add_subcategory", headers, new_sub.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          //test_attacks can be found on page
          resp = cl.Get("/test1/files/new_world/attacks", headers);
          REQUIRE(resp->body.find("test_attacks") != std::string::npos);
          //test attacks can be found
          resp = cl.Get("/test1/files/new_world/attacks/test_attacks", headers);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");

          //Check that game is still running.
          REQUIRE(system(command.c_str()) == 0);

          //Add a new object
          nlohmann::json new_obj;
          new_obj["name"] = "test_attack";
          new_obj["path"] = "/test1/files/new_world/attacks/test_attacks";
          resp = cl.Post("/api/add_object", headers, new_obj.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);
          //test_attacks can be found on page
          resp = cl.Get("/test1/files/new_world/attacks/test_attacks", headers);
          REQUIRE(resp->body.find("test_attack") != std::string::npos);
          //test attacks can be found
          resp = cl.Get("/test1/files/new_world/attacks/test_attacks/test_attack", 
              headers);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");
        }
        server.Stop();
    });
  t1.join();
  t2.join();
}
