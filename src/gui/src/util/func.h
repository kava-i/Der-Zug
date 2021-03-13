/**
 * @author: fux
 */
#ifndef SRC_SERVER_SRC_FUNC_H
#define SRC_SERVER_SRC_FUNC_H

#include <chrono>
#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <string.h>
#include <time.h>

#include <nlohmann/json.hpp>

namespace func 
{
  /**
  * @param[in] str
  */
  std::string ReturnToLower(std::string &str);

  /**
  * @param[in] str string to be splitet
  * @param[in] delimitter 
  * @return vector
  */
  std::vector<std::string> Split(std::string str, std::string delimiter);

  /**
   * Load a page (html/ css/ javascript) from disc and return as string
   * @param[in] path to file
   * @return complete file as string
   * Load the login
   */
  std::string GetPage(std::string path);

  /**    
   * Load an image from disc and return as string.      
   * @param[in] req (request)    
   * @param[in, out] resp (response)    
   * @param[in] path (path to image)                                 
   * @param[in] mime (immage type (jpg, png...)    
   */    
  std::string GetImage(std::string path);

  bool demo_exists(const std::filesystem::path& p, 
      std::filesystem::file_status s = std::filesystem::file_status{});

  /*
   * Load json from disc
   * @param[in] path (path to file)
   * @param[out] json (json to write data to)
   * @return boolean indicating success.
   */
  bool LoadJsonFromDisc(std::string path, nlohmann::json& json);

  /**
   * Write json to disc.
   * @param[in] path
   * @param[in] json to write
   * @return boolean indicating success.
   */
  bool WriteJsonToDisc(std::string path, nlohmann::json& json);

  std::string RemoveExtension(std::filesystem::path path);

  /**
   * Get current time in milliseconds.
   * @return long int (time in milliseconds)
   */
  long int TimeSinceEpoch();

  /**
   * @brief Implements a cryptographic hash function, uses the slower but better
   * SHA3 algorithm also named Keccak
   *
   * @param input The string to hash, for example password email whatever.
   *
   * @return The hashed string is returned, the input remains unchanged
   */
  std::string hash_sha3_512(const std::string& input);

  /**
   * Replaced non utf-8 characters. (ä->a, ß->ss etc. and for german characters
   * add 'e' for Umlaute and s to ß (ä->ae).
   * @param[in] str string to convert.
   * @return converted string.
   */
  std::string ReplaceNonUTF8(std::string& str);

  /**
   * Convert name to id. to lower and replaces " " with "_", also replaces non
   * utf8 characters.
   * @param[in] name to convert.
   * @return converted id.
   */
  std::string ConvertToId(std::string& name);

  /**
   * Convert id to name. First character of each word to Upper and replaces "_"
   * with " ".
   * @param[in] id to convert.
   * @return converted name.
   */
  std::string ConvertFromId(std::string &id);

  nlohmann::json ValidateJson(std::string json_string, std::vector<std::string> keys);
}

#endif
