/**
 * @author fux
 */
#ifndef SRC_TEXTADVENTURE_TOOLS_LOGICPARSER_H_
#define SRC_TEXTADVENTURE_TOOLS_LOGICPARSER_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "fuzzy.h"

class LogicParser{
  public:
    LogicParser() {
      opts_ = {'>', '<', '=', '~', ':'};
      opts_simple_ = {">", "<", "=", "~"};
    };

    /**
     * Contructor including the possibility to substitute certain strings
     * with given values (for instance you could map room to the current player
     * room by including {"room", [current_room]} to the map. Then room will 
     * be substituted by the value of [current_room]).
     * @param[in] substitute (map with possible substitutes)
     */
    LogicParser(std::map<std::string, std::string> substitute) {
      opts_ = {'>', '<', '=', '~', ':'};
      opts_simple_ = {">", "<", "=", "~"};
      substitue_ = substitute;
    }

    /**
     * return value of logic expression
     * Rules: only and (&) and or (|) allowed. String musst be formatted as 
     * binary tree: (a&(a&a)) NOT (a & a & a)
     * @param[in] input
     * @return true or false
     */
    bool Success(std::string input) {

      if (input == "") return true;

      //Check if already elementary form
      if (input.find("(") == std::string::npos) {
        auto vec = Split2(input);
        return Calc(vec[0], vec[1], vec[2]);
      }

      //Recursively create elementary form
      auto vec = GetBetween(input);
      if (vec[0].find("(") == std::string::npos && vec[2].find("(") 
          == std::string::npos)
        return Evaluate(vec[0], vec[1], vec[2]);
      if (vec[0].find("(") == std::string::npos)
        return Evaluate(vec[0], vec[1], Success(vec[2])); 
      else if (vec[2].find("(") == std::string::npos)
        return Evaluate(Success(vec[0]), vec[1], vec[2]);
      else 
        return Evaluate(Success(vec[0]), vec[1], Success(vec[2]));
    }

  private:

    std::map<std::string, std::string> substitue_;  ///< map with substitutes.
    std::vector<char> opts_;
    std::vector<std::string> opts_simple_;

    
    /**
     * Calculate value from elemenatry expressions.
     * Evaluate something like: "A" "=" "B"
     * Evaluate something like: "A = B|C"
     * @param[in] str1
     * @param[in] opt
     * @param[in] str2
     * @return boolean indicating value of expressions.
     */
    bool Calc(std::string str1, std::string opt, std::string str2) { 

      //Check if or ("|") is included in second string.
      if (str2.find("|") != std::string::npos) {
        return Calc(str1, opt, str2.substr(0, str2.find("|"))) ||
          Calc(str1, opt, str2.substr(str2.find("|")+1));
      }
      //Check if or ("&") is included in second string.
      if (str2.find("&") != std::string::npos) {
        return Calc(str1, opt, str2.substr(0, str2.find("&"))) ||
          Calc(str1, opt, str2.substr(str2.find("&")+1));
      }

      //Calculate value.
      if (std::find(opts_simple_.begin(), opts_simple_.end(), opt) 
          != opts_simple_.end()) {
        if (substitue_.count(str1) > 0) str1 = substitue_[str1];
        if (substitue_.count(str2) > 0) str2 = substitue_[str2];
        if (opt == "=") return str1 == str2;
        if (opt == "~") return fuzzy::fuzzy_cmp(str1, str2) <= 0.2;
        if (opt == ">") return std::stoi(str1) > std::stoi(str2);
        if (opt == "<") return std::stoi(str1) < std::stoi(str2);
      }
      else {
        if(opt == ":") return substitue_[str1].find(str2);
      }

      std::cout << "Wrong operand (=, ~, >, <)!" << std::endl;
      return true;
    }


    /**
     * @brief Evaluate something like: "(A = B) && (C = D)"
     * @param str1
     * @param opt
     * @param str2
     * @return 
     */
    bool Evaluate(std::string str1, std::string opt, std::string str2) { 
      auto vec1 = Split2(str1);
      auto vec2 = Split2(str2);
      return Evaluate(Calc(vec1[0], vec1[1], vec1[2]), opt, 
          Calc(vec2[0], vec2[1], vec2[2])); 
    }

    /**
     * @brief Evaluate something like: "A && (C = D)"
     * @param x
     * @param opt
     * @param str2
     * @return 
     */
    bool Evaluate(bool x, std::string opt, std::string str2) { 
      auto vec = Split2(str2);
      return Evaluate(x, opt, Calc(vec[0], vec[1], vec[2])); 
    }
    /**
     * @brief Evaluate something like: "(A = B) && C"
     * @param str1
     * @param opt
     * @param x
     * @return 
     */
    bool Evaluate(std::string str1, std::string opt, bool x) { 
      auto vec = Split2(str1);
      return Evaluate(Calc(vec[0], vec[1], vec[2]), opt, x); 
    }
    /**
     * @brief Evaluate something like: "(A && D)"
     * @param x
     * @param opt
     * @param y
     * @return 
     */
    bool Evaluate(bool x, std::string opt, bool y) { 
      if (opt == "|") return x || y;
      if (opt == "&") return x && y;
      std::cout << "Wrong operand! (|, &)!" << std::endl;
      return true; 
    }

    /**
     * @brief get position of next beginning expression
     * @param input
     * @return 
     */
    std::vector<std::string> GetBetween(std::string input) {
      size_t pos = input.find("(");

      std::vector<std::string> vec;
      if (pos == std::string::npos) 
        return Split2(input);
      else {
        pos = Split(input);

        std::string str = input.substr(pos+4);
        //Delete brackets of second string
        if (str.find("(") != std::string::npos) {
          str.erase(Split(str), 1);
          str.erase(str.find("("), 1);
        }
        //vec.push_back(str);
        return {input.substr(1, pos-1), input.substr(pos + 2, 1), str};
      }
    }

    /**
     * return position of where to split string
     */
    size_t Split(std::string str) {
      size_t num = 0;
      for (size_t x=0; x<str.length(); x++) {
        if (str[x] == '(')
          num++;
        else if (str[x] == ')' && num == 1)
          return x;
        else if (str[x] == ')')
          num--;
      }
      std::cout << "\nFailed!!\n";
      return 0;
    }

    /**
    * @param[in] str string to be splitet
    * @return vector
    */
    std::vector<std::string> Split2(std::string str) {
      std::string opt = "";
      size_t pos = 0;
      for (pos=0; pos<str.length(); pos++) {
        if (std::find(opts_.begin(), opts_.end(), str[pos]) != opts_.end()) {
          opt = str[pos];
          break;
        }
      }
      return {str.substr(0, pos), opt, str.substr(pos+1)};
    }
};

#endif
