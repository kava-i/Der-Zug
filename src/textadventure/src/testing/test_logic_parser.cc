#include "tools/logic_parser.h"
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <string>

TEST_CASE("Testadventure is starting", "[logic_parser]") {

	const std::map<std::string, std::string> status = {
		{"lp", "0"},
		{"ep", "25"},
		{"level", "-1"},
		{"room", "test.room1"},
		{"last_room", "test.room2"},
		{"inventory", "test.trank;test.room2.test.schwert"}
	};

  LogicParser logic(status);

	// Test basic assertions
	REQUIRE(logic.Success("lp=0"));
	REQUIRE(logic.Success("ep<30"));
	REQUIRE(logic.Success("ep>20"));
	REQUIRE(logic.Success("level=-1"));
	REQUIRE(logic.Success("room=test.room1"));


	// test fuzzy assertions
	REQUIRE(logic.Success("last_room~test.roum2"));

	// Test contains assertions
	REQUIRE(logic.Success("inventory:test.trank"));
	REQUIRE(!logic.Success("inventory:test.trank2"));
	REQUIRE(logic.Success("inventory:test.room2.test.schwert"));
	
	// test negation assertions
	REQUIRE(logic.Success("!inventory:test.room2.test.waffe"));
	REQUIRE(logic.Success("!lp=1"));
	REQUIRE(logic.Success("!ep<20"));
	REQUIRE(logic.Success("!room=test.room2"));
	REQUIRE(logic.Success("!last_room~room2.test"));

	// Test simplyfied OR
	REQUIRE(logic.Success("lp=0|1"));
	REQUIRE(logic.Success("lp=1|0"));
	REQUIRE(!logic.Success("lp=1|-1"));
	REQUIRE(logic.Success("inventory:test.room2.test.schwert|test.schwert"));
	
	// Test AND assertions
	REQUIRE(logic.Success("(lp=0)&&(ep<30)"));
	REQUIRE(logic.Success("(lp=0)&&(ep<30)"));
	REQUIRE(logic.Success("(lp=0)&&(inventory:test.room2.test.schwert)"));
	REQUIRE(logic.Success("(inventory:test.room2.test.schwert)&&(ep<30)"));
	REQUIRE(!logic.Success("(lp=1)&&(inventory:test.room2.test.schwert)"));
	REQUIRE(!logic.Success("(lp=0)&&(inventory:test.room2.test.waffe)"));

	// Test OR assertions
	REQUIRE(logic.Success("(lp=0)||(level=0)"));
	REQUIRE(logic.Success("(level=0)||(lp=0)"));
	REQUIRE(logic.Success("(lp>-1)||(level=<0)"));
	REQUIRE(logic.Success("(last_room~room2.test)||(inventory:test.trank)"));
	REQUIRE(logic.Success("(inventory:test.room2.test.waffe)||(last_room~test.roum2)"));

	// More Tests
	REQUIRE(logic.Success("(lp=0)&&((ep<20)||(ep>24))"));
	REQUIRE(!logic.Success("(lp=0)&&((ep<20)||(ep>25))"));
	REQUIRE(logic.Success("(inventory=test.trank)||((room=test.room1)&&(ep=25))"));
	REQUIRE(logic.Success("(inventory=test.waffe)||((room=test.room1)&&(!ep=21))"));
	REQUIRE(logic.Success("(lp<13)&&(last_room=test.trank"));

}
