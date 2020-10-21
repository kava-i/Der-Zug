/**
 * @author: fux
 */
#ifndef SRC_SERVER_SRC_FUNC_H
#define SRC_SERVER_SRC_FUNC_H

#include <chrono>
#include <codecvt>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <string.h>
#include <time.h>

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

  /**
   * Get current time in illiseconds.
   * @return long int (time in milliseconds)
   */
  int TimeSinceEpoch();

  /**
   * @brief Implements a cryptographic hash function, uses the slower but better
   * SHA3 algorithm also named Keccak
   *
   * @param input The string to hash, for example password email whatever.
   *
   * @return The hashed string is returned, the input remains unchanged
   */
  std::string hash_sha3_512(const std::string& input);
}

#endif