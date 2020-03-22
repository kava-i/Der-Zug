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
    vStr.push_back(str);

    return vStr;
}

/**
* @param[in, out] str string to be modified
*/
void func::convertToLower(std::string &str)
{
    std::locale loc1;
    for(unsigned int i=0; i<str.length(); i++)
        str[i] = tolower(str[i], loc1);
}

std::string func::returnToLower(std::string str)
{
    std::locale loc1;
    std::string str2;
    for(unsigned int i=0; i<str.length(); i++)
        str2 += tolower(str[i], loc1);
    return str2;
}

std::string func::returnToUpper(std::string str)
{
    std::locale loc1;
    std::string str2;
    for(unsigned int i=0; i<str.length(); i++)
        str2 += toupper(str[i], loc1);
    return str2;
}

std::string func::returnSwapedString(std::string str, int val)
{
    if(val==0)
        return str; 

    srand(time(NULL));
    std::vector<std::string> words = func::split(str, " ");

    size_t limit = (11-val)/2;

    size_t counter = 0;
    for(auto& word : words)
    {
        if(counter%(limit)!= 0) {
            counter++;
            continue;
        }

        for(size_t i=0; i<word.size(); i++) {

            if(i%(limit) != 0 || isalpha(word[i]) == false)
                continue;
            size_t num = rand() % word.size()-1;
            if(!isalpha(word[num]))
                continue;

            char x = word[i];
            word[i] = word[num];
            word[num] = x;
        }
        counter++;
    }

    std::string str2 ="";
    for(auto word : words)
        str2+=word + " ";
    return str2;
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
