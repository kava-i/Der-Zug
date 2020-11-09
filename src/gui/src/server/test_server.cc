/**
 * @author georgbuechner
 */
#include "util/func.h"
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
          resp = cl.Get("/overview", headers);
          REQUIRE(resp->status == 200);
          REQUIRE(resp->body != "");

          resp = cl.Post("/api/user_logout", headers, request.dump(), 
              "application/x-www-form-urlencoded");
          REQUIRE(resp->status == 200);

          //Now, overview page should not be accessable anymore
          resp = cl.Get("/overview", headers);
          REQUIRE(resp->status == 302);
        }
        server.Stop();
    });
  t1.join();
  t2.join();
}
