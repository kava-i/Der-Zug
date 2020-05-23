#include "func.hpp"
#include <catch2/catch.hpp>

/**
* @param[in] str string to be splitet
* @param[in] delimitter 
* @return vector
*/
std::vector<std::string> func::split(std::string str, std::string delimiter)
{ 
    std::vector<std::string> vStr;
    size_t pos=0;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        if(pos!=0)
            vStr.push_back(str.substr(0, pos));
        str.erase(0, pos + delimiter.length());
    }

    //Push ending of the string.
    vStr.push_back(str);

    return vStr;
}

/**
* @param[in] vec (vector to search string in )
* @param[in] str (string to search for)
* @return Return whether string occurs in vector.
*/
bool func::inArray(std::vector<std::string> vec, std::string str)
{
    for(const auto& it : vec) {
        if(it == str)
            return true;
    }
    return false;
}

/**
* Convert a given string to only contain lower case characters.
* @param[in, out] str string to be modified
*/
void func::convertToLower(std::string &str)
{
    std::locale loc1;
    for(unsigned int i=0; i<str.length(); i++)
        str[i] = tolower(str[i], loc1);
}

/**
* Convert a given string to only contain upper case characters.
* @param[in, out] str string to be modified
*/
void func::convertToUpper(std::string &str)
{
    std::locale loc1;
    for(unsigned int i=0; i<str.length(); i++)
        str[i] = toupper(str[i], loc1);
}

/**
* Return the given string to only contain lower case characters.
* @param str str to be converted.
* @return converted string in lower case.
*/
std::string func::returnToLower(std::string str)
{
    std::locale loc1;
    std::string str2;
    for(unsigned int i=0; i<str.length(); i++)
        str2 += tolower(str[i], loc1);
    return str2;
}

/**
* Return the given string to only contain upper-case characters.
* @param str string to be converted.
* @return converted string in upper-case.
*/
std::string func::returnToUpper(std::string str)
{
    std::locale loc1;
    std::string str2;
    for(unsigned int i=0; i<str.length(); i++)
        str2 += toupper(str[i], loc1);
    return str2;
}

/**
* Randomly swap a certain amount of characters in every nth word of a given string. 
* Number of characters to be swapped depends on given number ("num"). Higher number
* results in more characters being swapped in more words.
* @param str string in which characters will be swapped.
* @param num number indicating how many characters will be swapped in how many words.
*/
std::string func::returnSwapedString(std::string str, int val)
{
    //If val is 0 simply return str without any changes.
    if(val==0)
        return str; 

    //Set up random number generation and split string into separated words.
    srand(time(NULL));
    std::vector<std::string> words = func::split(str, " ");

    //Set limit
    size_t limit = (11-val)/2;

    //Iterate over all words
    size_t counter = 0;
    for(auto& word : words)
    {
        //Skip every nth word, where n=limit.
        if(counter%(limit)!= 0) {
            counter++;
            continue;
        }

        //Randomly swap characters from word.
        for(size_t i=0; i<word.size(); i++)
        {
            size_t num = rand() % word.size();
            if(i%(limit) != 0 || isalpha(word[i]) == false || isalpha(word[num]) == false)
                continue;

            char x = word[i]; word[i] = word[num]; word[num] = x;
        }
        counter++;
    }

    //Generate string to return
    std::string str2 ="";
    for(auto word : words)
        str2+=word + " ";
    return str2;
}

/**
* replace all entries in first json, which also occure in second json with
* this second value.
* @param[in] json1 (first json which will be updated)
* @param[in] json2 (second json)
*/
void func::updateJson(nlohmann::json& json1, nlohmann::json& json2)
{
    for(auto it = json2.begin(); it!=json2.end(); it++)
        json1[it.key()] = json2[it.key()];
}

int func::getNumFromBack(std::string str)
{
    int counter = 1; 
    int num = 0;
    while(std::isdigit(str.back()))
    {
        num += counter * ((char)str.back()-48);
        str.pop_back();
        counter *= 10;
    }
    return num;
}

std::string func::extractLeadingChars(const std::string& str)
{
    std::string sOpt;
    for(size_t i=0; i<str.length(); i++)
    {
        if(std::isdigit(str[i]) == false)
            sOpt+=str[i];
    }
    return sOpt; 
} 

/**
* Append to is highest postfix+1. Iterate over map and get highest postfix of similar ids.
* Return id + postfix increased by one.
* @param[in] mapObjects (map of all objects which might have a similar id
* @param[in] sID (id which shall be edited)
* @return id + greates postfix+1.
*/
std::string func::incIDNumber(std::map<std::string, std::string> mapObjects, std::string sID)
{
    int max = 0;
    bool found = false;
    for(const auto& it : mapObjects)
    {
        if(it.first.find(sID) != std::string::npos)
            found = true;
        if(it.first.find(sID) != std::string::npos && getNumFromBack(it.first) > max)
            max = getNumFromBack(it.first);
    } 
    if(max == 0 && found == false)
        return sID;
    else if(max == 0 && found == true)
        return sID + "2";
    else
        return sID + std::to_string(max+1);
}
