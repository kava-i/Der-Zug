/**
 * @author georgbuechner
 */
#include <iostream>

#include <catch2/catch.hpp>

#include "game/game.h"
#include "test_params.h"

TEST_CASE("Testadventure is starting", "[startup]") {
  
  TestParameters* params = params->getInstance();

  //Create game and all worlds.
  CGame game(params->txtad_path());

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

