/**
 * @author georgbuechner
 */
#define CATCH_CONFIG_MAIN

#include <iostream>
#include <thread>

#include <catch2/catch.hpp>
#include <httplib.h>

#include "server_frame.h"

void del_test_user(std::string username) {
  //If already exists, delete test data.
  try{
    std::filesystem::remove("../data/users/" + username + ".json");
  }
  catch(...) {return; }
  std::cout << username << " deleted." << std::endl;
}

TEST_CASE("Server is working as expected", "[server]") {
  
  del_test_user("test");
 
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
        server.Stop();
    });
  t1.join();
  t2.join();
}
