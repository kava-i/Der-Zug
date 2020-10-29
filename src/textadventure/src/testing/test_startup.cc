/**
 * @author georgbuechner
 */
#define CATCH_CONFIG_MAIN

#include <iostream>

#include <catch2/catch.hpp>

#include "game/game.h"

TEST_CASE("Testadventure is starting", "[startup]") {

  //Create game and all worlds.
  CGame game("../../data/users/test_manager/files/Test_World/");

  //Check that starting the game returns output.
  REQUIRE(game.startGame("", "test", nullptr) != "");

  //Test basic show functions.
  std::list<std::string> online_players;
  REQUIRE(game.play("show room", "test", online_players) != "");
  REQUIRE(game.play("show exits", "test", online_players) != "");
  REQUIRE(game.play("show people", "test", online_players) != "");
  REQUIRE(game.play("show items", "test", online_players) != "");
  REQUIRE(game.play("show details", "test", online_players) != "");
}

