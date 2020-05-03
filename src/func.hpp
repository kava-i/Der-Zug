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
    
    /**
    *                   ***     printList       ***
    * Print all elements from a directory with any type. Pass lambda function to specify 
    * out put for type apart from string, integer etc. It is also possible to pass a 
    * lambda function, as a condition, return either true or false, depending of whether
    * the objects meets certain criteria.
    * The objects will be printed as a list.
    * @param[in] std::map<std::string T1> map of elements
    * @param[in] std::function<T1(T1)> (optional lambda function - for printing custom objects)
    * @param[in] std::function<bool(T1)> (optional lambda function - condition)
    * @return list of elements as a list.
    */
    template <typename T1, typename T2=std::function<T1(T1)>, typename T3=std::function<bool(T1)>> 
    std::string printList(  std::map<std::string, T1> &list, 
                                T2 print = [](T1 t) -> T1 { return t; },
                                T3 condition = [](T1 t) -> bool { return true; }    )
    {
        std::string sOutput = "";

        //Iterate over map. When condition is met add word to output.
        for(auto [i, it] = std::tuple{1, list.begin()}; it!=list.end(); i++, it++)
        {
            //Check whether condition is met.
            if(condition(it->second) == true)
                sOutput += std::to_string(i) + ": " + print(it->second) + "\n";
            else
                --i;
        }
        return sOutput;
    }

    /**
    *                   ***     printProsa      ***
    * Tries to print objects of any type from a dictionary as a text. Optional extra 
    * lambda functions can be added, to changed what is printed for each objects, or only
    * print an object on certain conditions.
    * @param[in] std::map<std::string T1> map of elements
    * @param[in] std::function<T1(T1)> (optional lambda function - for printing custom objects)
    * @param[in] std::function<bool(T1)> (optional lambda function - condition)
    * @return List of elements in map as a text.
    */
    template<typename T1, typename T2=std::function<T1(T1)>, typename T3=std::function<bool(T1)>> 
        std::string printProsa( std::map<std::string, T1> &list,
                                T2 print = [](T1 t) -> T1 { return t; },
                                T3 condition = [](T1 t) -> bool {return true; } )
    {
        std::string sOutput = "";
        //Iterate over map. When condition is met, add "[word], " to output.
        for(auto [i, it] = std::tuple(0, list.begin()); it!=list.end(); i+=2, it++)
        {
            if(condition(it->second) == true)
                sOutput += print(it->second) + ", ";
        }

        //Erase last comma. 
        if(sOutput.rfind(",") != std::string::npos)
            sOutput.erase(sOutput.end()-2);

        //Replace last comma with "und"
        if(sOutput.rfind(",") != std::string::npos)
            sOutput.replace(sOutput.rfind(","), 2, " und ");

        //Erase last element (I don't now why this is necessary actually :D
        if(sOutput.back() == ' ')
            sOutput.pop_back();

        //Add dot at the end of text.
        if(sOutput.size() > 1)
            sOutput += ".";

        //Return string
        return sOutput;
    }

    
    /**
    *                   ***     getObjectId         ***
    * Check whether a given word matches any object of a map by value. If The value is a custom
    * object, use a lambda, to access the variable you want to check with. Comparison is done with
    * fuzzy search. Best match  or empty string is returned.
    * @param[in] mapObjects map of objects to be printed.
    * @param[in] sName name of searched object.
    * @return Return id of found object or empty string.
    */
    template<typename T1, typename T2=std::function<std::string(T1)>> 
    std::string getObjectId( std::map<std::string, T1> mapObjects, std::string sName, 
                               T2 getName = [](T1 t) -> std::string { return t; } )
    {
        //Check if name is an id and thus can be directly accessed and returned.
        if(mapObjects.count(sName) > 0)
            return sName;

        //Iterate over map and use fuzzy comparison to find matching name. Add all matches
        // to array of matches including the exact score.
        std::vector<std::pair<std::string, double>> matches;
        for(auto[i, it] = std::tuple{1, mapObjects.begin()}; it!=mapObjects.end(); i++, it++) {
            if(std::isdigit(sName[0]) && (i==stoi(sName)))
                return it->first;
            double match = fuzzy::fuzzy_cmp(getName(it->second), sName);
            if(match <= 0.2) 
                matches.push_back(std::make_pair(it->first, match));
        }

        //If no matches where found, return empty string.
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

    /**
    * Convert a map of objects to a map containing the id of the object as key 
    * and the attribute selected by the lambda-function as value.
    * @param[in] in map to convert.
    * @param[in] lambda expression to generate value.
    * @return converted map.
    */
    template<typename T1, typename T2 > 
    std::map<std::string, std::string> convertToObjectmap(std::map<std::string, T1> in, T2 lambda)
    {
        std::map<std::string, std::string> out;
        for(const auto &it : in)
            out[it.first] = lambda(it.second);
        return out;
    }

    /**
    * Convert map to vector.
    */
    template<typename T1, typename T2=std::function<std::string(T1)>>
    std::vector<std::string> to_vector(std::map<std::string, T1> map, 
                                        T2 lambda = [](T1 t) -> std::string { return t; })
    {
        std::vector<std::string> vec;
        for(const auto& it : map)
            vec.push_back(lambda(it.second));
        return vec;
    }
    

    /**
    * Print a given dictionary as html table.
    * @param[in] in dictionary to print as html table.
    * @param[in] lambda function to indicate, which attribute to print.
    * @param[in] style optional css styling.
    * @return html table as string.
    */
    template <typename T1, typename T2> 
    std::string table(std::map<std::string, T1> in, T2 &lambda, std::string style="width:10%")
    {
        //Generate html table.
        std::string str ="<table style='"+style+"'>";
        for(auto it : in) {
            str += "<tr><td>" + it.first + ":</td><td>";
            str += lambda(it.second);
            str += "</td></tr>";
        }
        str+="</table>";
        return str; 
    }

    /**
    * Print a given dictionary of dictionaries  as a table. With the option to highlight a certain
    * column in the table.
    * @param[in] in dictionary to print as html table.
    * @param[in] lambda1 function to indicate, which attribute to print.
    * @param[in] lambda2 function to indicate, which attribute to print.
    * @param[in] style optional css styling.
    * @param[in] highlight integer saying, which column to highlight.
    * @return html table as string.
    */
    template <typename T1, typename T2, typename T3> 
    std::string table(std::map<std::string, std::map<std::string, T1>> in, T2 &lamda1, T3 &lamda2, std::string style="width:10%", int highlight=-1)
    {
        //Generate table
        std::string str = "<table style='"+style+"'<tr>", color = "", white = Webcmd::set_color(Webcmd::color::WHITE);

        //Generate first row
        int counter = 0; size_t max = 0;
        for(auto& it : in){
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

        //Generate other rows
        for(size_t i=0; i<max; i++)
        {
            counter = 0; str += "<tr>";
            for(auto it : in) {
                if(counter == highlight) color = Webcmd::set_color(Webcmd::color::GREEN);
                else color = "";

                //Check if current column is empty
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

