#pragma once

#include <iostream>
#include <string.h>
#include <string>
#include <map>
#include <list>
#include <locale>
#include <regex>
#include <time.h>
#include <ctime>
#include "json.hpp"

#include "fuzzy.hpp"

namespace func 
{
    /**
    * @param[in] str string to be splitet
    * @param[in] delimitter 
    * @return vector
    */
    std::vector<std::string> split(std::string str, std::string delimiter);

    /**
    * @param[in] vec (vector to search string in )
    * @param[in] str (string to search for)
    * @return Return whether string occurs in vector.
    */
    bool inArray(std::vector<std::string> vec, std::string str);

    /**
    * Convert a given string to only contain lower case characters.
    * @param[in, out] str string to be modified
    */
    void convertToLower(std::string &str);

    /**
    * Convert a given string to only contain upper case characters.
    * @param[in, out] str string to be modified
    */
    void convertToUpper(std::string &str);

    /**
    * Return the given string to only contain lower case characters.
    * @param str string to be converted.
    * @return converted string in lower case.
    */
    std::string returnToLower(std::string str);

    /**
    * Return the given string to only contain upper-case characters.
    * @param str string to be converted.
    * @return converted string in upper-case.
    */
    std::string returnToUpper(std::string str);


    /**
    * Convert a string to number. Starts from the back, stops at first non-digit character 
    * from the back.
    * @param[in] str (source string)
    * @return number
    */
    int getNumFromBack(std::string str);


    /**
    * Extracts all leading chars before some digits
    * @param[in] str (source string) 
    * @return leading chars
    */
    std::string extractLeadingChars(const std::string& str);


    /**
    *                   *** returnSwapedString  ***
    * Randomly swap a certain amount of characters in every nth word of a given string. 
    * Number of characters to be swapped depends on given number ("num"). Higher number
    * results in more characters being swapped in more words.
    * @param str string in which characters will be swapped.
    * @param num number indicating how many characters will be swapped in how many words.
    */
    std::string returnSwapedString(std::string str, int num);

    /**
    * replace all entries in first json, which also occure in second json with
    * this second value.
    * @param[in] json1 (first json which will be updated)
    * @param[in] json2 (second json)
    */
    void updateJson(nlohmann::json& json1, nlohmann::json& json2);

    /**
    * Append to is highest postfix+1. Iterate over map and get highest postfix of similar ids.
    * Return id + postfix increased by one.
    * @param[in] mapObjects (map of all objects which might have a similar id
    * @param[in] sID (id which shall be edited)
    * @return id + greates postfix+1.
    */
    std::string incIDNumber(std::map<std::string, std::string> mapObjects, std::string sID);
}

