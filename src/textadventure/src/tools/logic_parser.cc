#include "logic_parser.h"

LogicParser::LogicParser() {
  opts_ = {'>', '<', '=', '~', ':'};
  opts_simple_ = {">", "<", "=", "~"};
}

LogicParser::LogicParser(std::map<std::string, std::string> substitute) {
  opts_ = {'>', '<', '=', '~', ':'};
  opts_simple_ = {">", "<", "=", "~"};
  substitue_ = substitute;
}

bool LogicParser::Success(std::string input) {
  if (input == "") return true;

  DeleteNonsense(input);
  
  // Check if elementery form but in brackets
  if (input.front() == '(' && MatchingBracket(input) == input.length()-1) {
      //&& NumFind(input, " | ") < 1 && MatchingBracket(input, " & ") < 1) {
    input.erase(0, 1);
    input.pop_back();
  }

  // Check if already elementary form
  if (input.find("(") == std::string::npos) {
    auto vec = MatchingBracketAtOperator(input);
    return Calc(vec[0], vec[1], vec[2]);
  }

  // Recursively create elementary form
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

bool LogicParser::Calc(std::string str1, std::string opt, std::string str2) { 
	std::cout << "LogicParser::Calc: " << str1 << ", " << opt << ", " << str2  << std::endl;
  //Check if or ("|") is included in second string.
  if (str2.find("|") != std::string::npos) {
    return Calc(str1, opt, str2.substr(0, str2.find("|"))) ||
      Calc(str1, opt, str2.substr(str2.find("|")+1));
  }
  //Check if or ("&") is included in second string.
  if (str2.find("&") != std::string::npos) {
    return Calc(str1, opt, str2.substr(0, str2.find("&"))) &&
      Calc(str1, opt, str2.substr(str2.find("&")+1));
  }

  //Check if term shall be negated
  bool negation = true;
  if (str1.front() == '!') {
    negation = false;
    str1.erase(0,1);
  }

  // Substitut first or second literal
  if (str1.front() == '\'')
    str1.erase(0,1);
  else if (substitue_.count(str1) > 0)
    str1 = substitue_[str1];
  if (str2.front() == '\'') {
    str2.erase(0,1);
		str2.pop_back();
	}
  else if (substitue_.count(str2) > 0) {
    str2 = substitue_[str2];
	}

	std::cout << "LogicParser: After subsituting: " << str1 << ", " << opt << ", " << str2  << std::endl;

  //Calculate value.
  bool result = true;
  if (opt == "=") 
		result = str1 == str2;
  else if (opt == "~") 
    result = fuzzy::fuzzy_cmp(str2, str1) <= 0.2;
  else if (opt == ">") 
    result = std::stoi(str1) > std::stoi(str2);
  else if (opt == "<") 
    result = std::stoi(str1) < std::stoi(str2);
  else if(opt == ":") {
    auto vec = Split(str1, ";");
		std::cout << "LP: Checking against list: " << str2 << std::endl;
		for (const auto& it : vec) 
			std::cout << "LP: - " << it << std::endl;
    result = std::find(vec.begin(), vec.end(), str2) != vec.end();
  }
  else
    std::cout << "Wrong operand (=, ~, >, <)!" << std::endl;

  if (negation == true) return result;
  return !result;
}

bool LogicParser::Evaluate(std::string str1, std::string opt, std::string str2) {
  auto vec1 = MatchingBracketAtOperator(str1);
  auto vec2 = MatchingBracketAtOperator(str2);
  return Evaluate(Calc(vec1[0], vec1[1], vec1[2]), opt, 
      Calc(vec2[0], vec2[1], vec2[2])); 
}

bool LogicParser::Evaluate(bool x, std::string opt, std::string str2) {
  auto vec = MatchingBracketAtOperator(str2);
  return Evaluate(x, opt, Calc(vec[0], vec[1], vec[2])); 
}

bool LogicParser::Evaluate(std::string str1, std::string opt, bool x) {
  auto vec = MatchingBracketAtOperator(str1);
  return Evaluate(Calc(vec[0], vec[1], vec[2]), opt, x); 
}

bool LogicParser::Evaluate(bool x, std::string opt, bool y) { 
  if (opt == "|") return x || y;
  if (opt == "&") return x && y;
  std::cout << "Wrong operand! (|, &)!" << std::endl;
  return true; 
}

std::vector<std::string> LogicParser::GetBetween(std::string input) {
  size_t pos = input.find("(");
  if (pos == std::string::npos) 
    return MatchingBracketAtOperator(input);
  else {
    pos = MatchingBracket(input);
    std::string str = input.substr(pos+3);
    return {input.substr(1, pos-1), input.substr(pos + 2, 1), str};
  }
}

size_t LogicParser::MatchingBracket(std::string str) {
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

std::vector<std::string> LogicParser::MatchingBracketAtOperator(std::string 
    str) {
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

size_t LogicParser::NumFind(std::string str, std::string m) {
  size_t num=0;
  size_t pos = str.find(m);
  while (pos != std::string::npos) {
    num++;
    pos = str.find(m, pos+m.length());
  }
  return num;
}

std::vector<std::string> LogicParser::Split(std::string str, std::string 
    delimiter) {
  std::vector<std::string> strings;
  size_t pos=0;
  while ((pos = str.find(delimiter)) != std::string::npos) {
    if(pos!=0)
      strings.push_back(str.substr(0, pos));
    str.erase(0, pos + delimiter.length());
  }

  //Push ending of the string and return
  strings.push_back(str);
  return strings;
}

void LogicParser::DeleteNonsense(std::string& str) {
  for(;;) {
    if 
      (str.front() == ' ') str.erase(0, 1);
    else if 
      (str.back() == ' ') str.pop_back();
    else 
      break;
  }
}
