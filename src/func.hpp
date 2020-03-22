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

#include "fuzzy.hpp"
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

    std::string returnSwapedString(std::string str, int num);

    /**
    * Print all elements from a directory with any type. Pass lamda function to specify 
    * out put for type apartfrom string, interger etc. It is also possible to pass a 
    * lamda function, as a condition, return either true or false, depending of whether
    * the objects meets certain criteria.
    * The objects will be printed as a list.
    * @param[in] std::map<std::string T1> map of elememts
    * @param[in] std::function<T1(T1)> (optional lamda function - for printing custom objects)
    * @param[in] std::function<bool(T1)> (optional lamda function - condition)
    */
    template <typename T1, typename T2=std::function<T1(T1)>, typename T3=std::function<bool(T1)>> 
        std::string printList(  std::map<std::string, T1> &list, 
                                T2 print = [](T1 t) -> T1 { return t; },
                                T3 condition = [](T1 t) -> bool { return true; }    )
    {
        std::string sOutput = "";
        for(auto [i, it] = std::tuple{1, list.begin()}; it!=list.end(); i++, it++)
        {
            if(condition(it->second) == true)
                sOutput += std::to_string(i) + ": " + print(it->second) + "\n";
            else
                --i;
        }
        return sOutput;
    }

    /**
    * Tries to print objects of any type from a dictionary as a text. Optionall extra 
    * lamdafuntions can be added, to changed what is printed for each objects, or only
    * print an object on certain conditions.
    * @param[in] std::map<std::string T1> map of elememts
    * @param[in] std::function<T1(T1)> (optional lamda function - for printing custom objects)
    * @param[in] std::function<bool(T1)> (optional lamda function - condition)
    */
    template<typename T1, typename T2=std::function<T1(T1)>, typename T3=std::function<bool(T1)>> 
        std::string printProsa( std::map<std::string, T1> &list,
                                T2 print = [](T1 t) -> T1 { return t; },
                                T3 condition = [](T1 t) -> bool {return true; } )
    {
        std::string sOutput = "";
        for(auto [i, it] = std::tuple(0, list.begin()); it!=list.end(); i+=2, it++)
        {
            if(condition(it->second) == true)
                sOutput += print(it->second) + ", ";
        }
        if(sOutput.rfind(",") != std::string::npos)
            sOutput.erase(sOutput.end()-2);
        if(sOutput.rfind(",") != std::string::npos)
            sOutput.replace(sOutput.rfind(","), 2, " und ");
        if(sOutput.back() == ' ')
            sOutput.pop_back();
        return sOutput;
    }

    
    /**
    * check whether a given word matches any object of a map by value. If The value is a custom
    * object, use a lamda, to access the variable you want to check with. Comparrison is done withe
    * fuzzy search. Best match  or empty string is returned.
    */
    template<typename T1, typename T2=std::function<std::string(T1)>> 
    std::string getObjectId( std::map<std::string, T1>& mapObjects, std::string sName, 
                               T2 getName = [](T1 t) -> std::string { return t; } )
    {
        if(mapObjects.count(sName) > 0)
            return sName;

        std::vector<std::pair<std::string, double>> matches;
        
        for(auto[i, it] = std::tuple{1, mapObjects.begin()}; it!=mapObjects.end(); i++, it++) {
            if(std::isdigit(sName[0]) && (i==stoi(sName)))
                return it->first;
            double match = fuzzy::fuzzy_cmp(getName(it->second), sName);
            if(match <= 0.2) 
                matches.push_back(std::make_pair(it->first, match));
        }

        if(matches.size() == 0)
            return "";

        //Find best match.
        size_t pos=0;
        for(auto[i, max] = std::tuple{size_t{0}, 0.3}; i<matches.size(); i++) {
            if(matches[i].second < max) {
                pos=i;
                max=matches[i].second;
            }
        }
        return matches[pos].first;
    }

    typedef std::map<std::string, std::string> objectmap;
    template<typename T1, typename T2> 
    objectmap convertToObjectmap(std::map<std::string, T1> in, T2 &lamda)
    {
        objectmap out;
        for(const auto &it : in)
            out[it.first] = lamda(it.second);
        return out;
    }
    

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

