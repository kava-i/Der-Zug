/**
 * @author georgbuechner
 */
#include <iostream>

#include <catch2/catch_test_macros.hpp>

#include "game/game.h"
#include "test_params.h"

TEST_CASE("Testadventure is starting", "[startup]") {
  
  TestParameters* params = TestParameters::getInstance();
	std::cout << "PLAYER: " << params->txtad_player() << std::endl;
	if (params->txtad_player() == "")
		return;

  //Create game and all worlds.
  CGame game(params->txtad_path());
  game.SetupGame();

  //Check that starting the game returns output.
  REQUIRE(game.startGame(params->txtad_player(), nullptr) != "");

  //Test basic show functions.
  std::list<std::string> online_players;
  REQUIRE(game.play("show room", params->txtad_player(), online_players) != "");
  REQUIRE(game.play("show exits", params->txtad_player(), online_players) != "");
  REQUIRE(game.play("show people", params->txtad_player(), online_players) != "");
  REQUIRE(game.play("show items", params->txtad_player(), online_players) != "");
  REQUIRE(game.play("show details", params->txtad_player(), online_players) != "");
}

