
#include "webcmd.h"


std::string Webcmd::set_color(color cl)
{
    char ch = cl;
    std::string x;
    x.push_back(ch);
    return x;
}

std::string Webcmd::set_sound(std::string filename)
{
    char ch = 7;
    std::string ret;
    ret.push_back(ch);
    ret+=filename;
    ret.push_back(ch);
    return ret;
}
