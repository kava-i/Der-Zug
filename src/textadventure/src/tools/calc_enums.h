#pragma once

#include <functional>
#include <map>
#include <string>

namespace calc {
	enum MatchType {
		EQUALS,
		GREATER, 
		GREATER_THAN, 
		LESSER,
		LESSER_THAN,
		UNEQUAL,
	};

	const std::map<std::string, MatchType> MATCH_TYPE_STRING_MAPPING = {
		{"=", EQUALS}, {">", GREATER}, {">=", GREATER_THAN}, {"<", LESSER}, {"<=", LESSER_THAN}, {"!=", UNEQUAL},
	};

	const std::map<MatchType, std::function<bool(const int&, const int&)>> MATCH_TYPE_FUNCTION_MAPPING = {
		{EQUALS, [](const int& a, const int& b) { return a==b; } },
		{GREATER, [](const int& a, const int& b) { return a>b; } },
		{GREATER_THAN, [](const int& a, const int& b) { return a>=b; } },
		{LESSER, [](const int& a, const int& b) { return a<b; } },
		{LESSER_THAN, [](const int& a, const int& b) { return a<=b; } },
		{UNEQUAL, [](const int& a, const int& b) { return a!=b; } },
	};

	enum ModType {
		SET,
		ADD,
		MIN, 
		MULT, 
		DIV,
	};

	const std::map<std::string, ModType> MOD_TYPE_STRING_MAPPING = {
		{"=", SET}, {"+", ADD}, {"-", MIN}, {"*", MULT}, {"/", DIV},
	};

	const std::map<ModType, std::string> MOD_TYPE_R_STRING_MAPPING = {
		{SET, "="}, {ADD, "+"}, {MIN, "-"}, {MULT, "*"}, {DIV, "/"},
	};

  const std::map<ModType, ModType> MOD_TYPE_R_MAPPING = {
    {SET, SET}, {ADD, MIN}, {MIN, ADD}, {MULT, DIV}, {DIV, MULT}
  };

	const std::map<ModType, std::function<void(int& a, const int& b)>> MOD_TYPE_FUNCTION_MAPPING = {
		{SET, [](int& a, const int& b) { a=b;} },
		{ADD, [](int& a, const int& b) { a+=b;} },
		{MIN, [](int& a, const int& b) { a-=b;} },
		{MULT,[](int& a, const int& b) { a*=b;} },
		{DIV, [](int& a, const int& b) { a/=b;} },
	};

	const std::map<ModType, std::string> MOD_TYPE_MSG_MAPPING = {
		{SET, " set to "},
		{ADD, " increased by " },
		{MIN, " decreased by " },
		{MULT, " increased by " },
		{DIV, " decreased by " },
	};
}



