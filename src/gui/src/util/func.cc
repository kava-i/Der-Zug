#include "func.h"
#include "cleanup_dtor.h"
 
#include <openssl/evp.h>
#include <openssl/sha.h>

namespace fs = std::filesystem;

namespace func 
{

std::string ReturnToLower(std::string &str) {
  std::string str2;
  for (unsigned int i=0; i<str.length(); i++)
    str2 += tolower(str[i]);

  return str2;
}

std::vector<std::string> Split(std::string str, std::string delimiter) {
  std::vector<std::string> v_strs;
  size_t pos=0;
  while ((pos = str.find(delimiter)) != std::string::npos) {
    if(pos!=0)
        v_strs.push_back(str.substr(0, pos));
    str.erase(0, pos + delimiter.length());
  }
  v_strs.push_back(str);

  return v_strs;
}

std::string GetPage(std::string path) {
  //Read loginpage and send
  std::ifstream read(path);
  if (!read) {
    std::cout << "Wrong file passed: " << path << std::endl;
    return "";
  }
  std::string page( (std::istreambuf_iterator<char>(read) ),
                    std::istreambuf_iterator<char>()     );
  //Delete file-end marker
  page.pop_back();
  return page;
}

std::string GetImage(std::string path) {
  std::ifstream f(path, std::ios::in|std::ios::binary|std::ios::ate);    
  if (!f.is_open()) {
    std::cout << "file could not be found!" << std::endl;    
    return "";
  }
  FILE* file_stream = fopen(path.c_str(), "rb");    
  std::vector<char> buffer;    
  fseek(file_stream, 0, SEEK_END);    
  long length = ftell(file_stream);    
  rewind(file_stream);    
  buffer.resize(length);    
  length = fread(&buffer[0], 1, length, file_stream);    
      
  std::string s(buffer.begin(), buffer.end());    
  return s;
}

bool demo_exists(const fs::path& p, fs::file_status s) {
  if (fs::status_known(s) ? fs::exists(s) : fs::exists(p)) 
    return true;
  else
    return false;
}

nlohmann::json LoadJsonFromDisc(std::string path) {
  if (!demo_exists(path))
    return nlohmann::json({{"error", "path not found!"}});
  nlohmann::json json;
  std::ifstream read(path);
  read >> json;
  read.close();
  return json;
}

int TimeSinceEpoch() {
  std::chrono::milliseconds ms = std::chrono::duration_cast
    <std::chrono::milliseconds >
    (std::chrono::system_clock::now().time_since_epoch());
  return ms.count();
}

std::string hash_sha3_512(const std::string& input) {
  unsigned int digest_length = SHA512_DIGEST_LENGTH;
  const EVP_MD* algorithm = EVP_sha3_512();
  uint8_t* digest = static_cast<uint8_t*>(OPENSSL_malloc(digest_length));
  CleanupDtor dtor([digest](){OPENSSL_free(digest);});


  EVP_MD_CTX* context = EVP_MD_CTX_new();
  EVP_DigestInit_ex(context, algorithm, nullptr);
  EVP_DigestUpdate(context, input.c_str(), input.size());
  EVP_DigestFinal_ex(context, digest, &digest_length);
  EVP_MD_CTX_destroy(context);

  std::stringstream stream;
  stream<<std::hex;

  for(auto b : std::vector<uint8_t>(digest,digest+digest_length))
    stream << std::setw(2) << std::setfill('0') << (int)b;

  return stream.str();
}

} //Close namespace 
