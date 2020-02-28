#pragma once

#include <iostream>
#include <string.h>
#include <string>
#include <map>
#include <list>
#include <locale>
#include <regex>
#include "Webcmd.hpp"

namespace func 
{
    /**
    * @param[in] str string to be splitet
    * @param[in] delimitter 
    * @return vector
    */
    std::vector<std::string> split(std::string str, std::string delimiter);

    /**
    * @param[in, out] str string to be modified
    */
    void convertToLower(std::string &str);

    std::string returnToLower(std::string str);
    std::string returnToUpper(std::string str);

    template <typename T1, typename T2> std::string table(std::map<std::string, T1> t, T2 &lamda, std::string style="width:10%")
    {
        std::string str ="<table style='"+style+"'>";
        for(auto it : t) {
            str += "<tr><td>" + it.first + ":</td><td>";
            str += lamda(it.second);
            str += "</td></tr>";
        }
        str+="</table>";
        return str; 
    }

    template <typename T1, typename T2, typename T3> std::string table(std::map<std::string, std::map<std::string, T1>> t, T2 &lamda1, T3 &lamda2, std::string style="width:10%", int highlight=-1)
    {
        std::string str = "<table style='"+style+"'<tr>", color = "", white = Webcmd::set_color(Webcmd::color::WHITE);

        int counter = 0; size_t max = 0;
        for(auto& it : t){
            color = ""; 
            if(it.second.size() > max)
                max = it.second.size();
            if(counter == highlight)
                color = Webcmd::set_color(Webcmd::color::GREEN);
            str += "<th>" + color + it.first + "</th><th></th>";
            str += Webcmd::set_color(Webcmd::color::WHITE);
            counter++;
        }
        str += "</tr>";

        for(size_t i=0; i<max; i++)
        {
            counter = 0; str += "<tr>";
            for(auto it : t) {
                if(counter == highlight) color = Webcmd::set_color(Webcmd::color::GREEN);
                else color = "";

                if(it.second.size() <= i)
                    str +="<td></td><td></td>";
                else {
                    auto jt = it.second.begin(); std::advance(jt, i);
                    str += "<td>" + color + lamda1(jt->first, jt->second) +  ":</td>"
                         + "<td>" + color + lamda2(jt->first, jt->second) + "</td>";
                }
                str += Webcmd::set_color(Webcmd::color::WHITE);
                counter++;
            }
            str += "</tr>";
        }
        return str + "</table>"; 
    }
}

