/**
 * @author georgbuechner
 */
#define CATCH_CONFIG_MAIN

#include <iostream>

#include <catch2/catch.hpp>

#include "game/game.h"

TEST_CASE("Testadventure is starting", "[startup]") {
  CGame("../../data/users/test_manager/files/Test_World/");

}

