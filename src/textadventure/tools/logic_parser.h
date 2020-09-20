/**
 * @author fux
 */
#ifndef SRC_TEXTADVENTURE_TOOLS_LOGICPARSER_H_
#define SRC_TEXTADVENTURE_TOOLS_LOGICPARSER_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "fuzzy.h"
//#include "func.h"

class LogicParser{
  public:
    LogicParser() {
      substitue_ = {
        {"cmd", "show"},
        {"input", "zu gleis"},
        {"room", "h_trainstation"}, 
        {"logic", "5"}};

    };

    bool Success(std::string input) {
      if (input.find("(") == std::string::npos) {
        return Calc(Split2(input)[0], Split2(input)[1], 
            Split2(input)[2]);
      }
      std::string str1 = GetBetween(input)[0];
      std::string opt  = GetBetween(input)[1];
      std::string str2 = GetBetween(input)[2];
        
      if (str1.find("(") == std::string::npos && str2.find("(") == std::string::npos)
        return Evaluate(str1, opt, str2);
      if (str1.find("(") == std::string::npos)
        return Evaluate(str1, opt, Success(str2)); 
      else if (str2.find("(") == std::string::npos)
        return Evaluate(Success(str1), opt, str2);
      else 
        return Evaluate(Success(str1), opt, Success(str2));
    }

  private:

    std::map<std::string, std::string> substitue_;

    //Evaluate something like: "A" "=" "B"
    bool Calc(std::string str1, std::string opt, std::string str2) { 
      if (substitue_.count(str1) > 0) str1 = substitue_[str1];
      if (substitue_.count(str2) > 0) str2 = substitue_[str2];
      if (opt == "=") return str1 == str2;
      if (opt == "~") return fuzzy::fuzzy_cmp(str1, str2) <= 0.2;
      if (opt == ">") return std::stoi(str1) > std::stoi(str2);
      if (opt == "<") return std::stoi(str1) < std::stoi(str2);
      std::cout << "Wrong operand" << std::endl;
      return true;
    }
    bool Evaluate(std::string str1, std::string opt, std::string str2) { 
      auto vec1 = Split2(str1);
      auto vec2 = Split2(str2);
      return Evaluate(Calc(vec1[0], vec1[1], vec1[2]), opt, 
          Calc(vec2[0], vec2[1], vec2[2])); 
    }

    bool Evaluate(bool x, std::string opt, std::string str2) { 
      auto vec = Split2(str2);
      return Evaluate(x, opt, Calc(vec[0], vec[1], vec[2])); 
    }
    bool Evaluate(std::string str1, std::string opt, bool x) { 
      auto vec = Split2(str1);
      return Evaluate(Calc(vec[0], vec[1], vec[2]), opt, x); 
    }
    bool Evaluate(bool x, std::string opt, bool y) { 
      if (opt == "|") return x || y;
      if (opt == "&") return x && y;
      std::cout << "Wrong operand! (|, &)" << std::endl;
      return true; 
    }

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
      std::vector<char> operands = {'>', '<', '=', '~'};
      std::string opt = "";
      size_t pos = 0;
      for (size_t i; i<str.length(); i++) {
        if (std::find(operands.begin(), operands.end(), str[i]) != operands.end()) {
          opt = str[i];
          pos = i;
          break;
        }
      }
      return {str.substr(0, pos), opt, str.substr(pos+1)};
    }
};

#endif
