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
        std::cout << it.c_str() << " | " << str.c_str() << " -> ";
        if(it == str)
        {
            std::cout << "true" << std::endl;
            return true;
        }
        std::cout << "false" << std::endl;
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
            size_t num = rand() % word.size()-1;
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

TEST_CASE("Testing func::split functionality","[func::split]")
{
    std::string lk="Hallo ich funktioniere";
    auto vec = func::split(lk," ");
    REQUIRE(vec.size() == 3);
    REQUIRE(vec[0] == "Hallo");
    REQUIRE(vec[1] == "ich");
    REQUIRE(vec[2] == "funktioniere");

    vec = func::split(lk,"ich");
    REQUIRE(vec.size() == 2);
    REQUIRE(vec[0] == "Hallo ");
    REQUIRE(vec[1] == " funktioniere");
}

TEST_CASE("Testing func::convertToLower","[func::overtToLower]")
{
    std::string main="Alex";
    func::convertToLower(main);
    REQUIRE(main=="alex");

    main="AlExAnDeR";
    func::convertToLower(main);
    REQUIRE(main=="alexander");
}
