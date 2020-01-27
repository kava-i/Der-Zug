#include "func.hpp"
#include "Catch2/single_include/catch2/catch.hpp"

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
