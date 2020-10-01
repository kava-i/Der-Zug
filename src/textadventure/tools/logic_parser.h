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

class LogicParser{
  public:
    /**
     * Constructor stetting up operands and simple operands.
     */
    LogicParser();

    /**
     * Contructor including the possibility to substitute certain strings
     * with given values (for instance you could map room to the current player
     * room by including {"room", [current_room]} to the map. Then room will 
     * be substituted by the value of [current_room]).
     * @param[in] substitute (map with possible substitutes)
     */
    LogicParser(std::map<std::string, std::string> substitute);

    /**
     * return value of logic expression
     * Rules: only and (&) and or (|) allowed. String musst be formatted as 
     * binary tree: (a&(a&a)) NOT (a & a & a)
     * @param[in] input
     * @return true or false
     */
    bool Success(std::string input);

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
    bool Calc(std::string str1, std::string opt, std::string str2);

    /**
     * @brief Evaluate something like: "(A = B) && (C = D)"
     * @param str1
     * @param opt
     * @param str2
     * @return 
     */
    bool Evaluate(std::string str1, std::string opt, std::string str2);

    /**
     * @brief Evaluate something like: "A && (C = D)"
     * @param x
     * @param opt
     * @param str2
     * @return 
     */
    bool Evaluate(bool x, std::string opt, std::string str2);
    
    /**
     * @brief Evaluate something like: "(A = B) && C"
     * @param str1
     * @param opt
     * @param x
     * @return 
     */
    bool Evaluate(std::string str1, std::string opt, bool x);

    /**
     * @brief Evaluate something like: "(A && D)"
     * @param x
     * @param opt
     * @param y
     * @return 
     */
    bool Evaluate(bool x, std::string opt, bool y);

    /**
     * @brief get position of next beginning expression
     * @param input
     * @return 
     */
    std::vector<std::string> GetBetween(std::string input);

    /**
     * return position of last closing bracket.
     */
    size_t MatchingBracket(std::string str);

    /**
    * @param[in] str string to be splitet
    * @return vector
    */
    std::vector<std::string> MatchingBracketAtOperator(std::string str);

    /**
     * Return number of occurances of substring in given string
     * @param[in] str (given string). 
     * @param[in] m (string to match).
     * @return occurances
     */
  size_t NumFind(std::string str, std::string m);

  /**
  * @param[in] str string to be splitet
  * @param[in] delimitter 
  * @return vector
  */
  std::vector<std::string> Split(std::string str, std::string delimiter);

  /**
   * Delete spaces at the front and back.
   * @param[in, out] str (string to modify)
   */
  void DeleteNonsense(std::string& str);
};

#endif
