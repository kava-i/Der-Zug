#include "func.h"
#include "cleanup_dtor.h"

#include <codecvt>
 
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/sha.h>

namespace fs = std::filesystem;

namespace func 
{

std::string ReturnToLower(std::string &str) {
  std::locale loc("de_DE.UTF-8");
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring wide = converter.from_bytes(str);
  std::wstring wide2;
  for (unsigned int i=0; i<wide.length(); i++)
    wide2 += tolower(wide[i], loc);

  std::string str2 = converter.to_bytes(wide2);
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

// Load the markdown file as a string
std::string LoadMarkdown(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
		std::string str = buffer.str();


    // Escape newlines in the string
    std::string escaped_str;
    for (char c : str) {
        if (c == '\n') {
            escaped_str += "\\n";
        } else {
            escaped_str += c;
        }
    }

    return escaped_str;
}

std::string GetPage(std::string path) {
  std::cout << "GetPage: " << path << std::endl;
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

std::string GetMedia(std::string path) {
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

void StoreMedia(std::string path, std::string content) {
  try {
    std::fstream mediaout(path, std::ios::out | std::ios::binary);
    mediaout.write(content.c_str(), content.size());
    mediaout.close();
  }
  catch (std::exception& e) {
    std::cout << "Writing media-file failed: " << e.what() << std::endl;
    return;
  }
  std::cout << "Writing media-file success." << std::endl;
}

bool demo_exists(const fs::path& p, fs::file_status s) {
  if (fs::status_known(s) ? fs::exists(s) : fs::exists(p)) 
    return true;
  else
    return false;
}

bool LoadJsonFromDisc(std::string path, nlohmann::json& json) {
  if (!demo_exists(path)) {
    std::cout << "Path: " << path << " not found!" << std::endl; 
    return false;
  }
  std::ifstream read(path);
  try {
    read >> json;
    read.close();
    return true;
  }
  catch (std::exception& e) {
    std::cout << "Failed loading object: " << e.what() << std::endl;
    read.close();
    return false;
  }
}

bool WriteJsonToDisc(std::string path, nlohmann::json& json) {
  try {
    std::ofstream write(path);
    write << json;
    write.close();
    return true;
  }
  catch (std::exception& e) {
    std::cout << "Failed writing object: " << e.what() << std::endl;
    return false;
  }
}

std::string RemoveExtension(std::filesystem::path path) {
  return path.replace_extension("");
}

long int TimeSinceEpoch() {
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

std::string ReplaceNonUTF8(std::string& str) {
  std::map<wchar_t, char> rep = {{L'ä','a'},{L'ö','o'},{L'ü','u'},{L'ß','s'},{L'é','e'},{L'è','e'},{L'á','a'},{L'ê','e'},{L'â','a'}, {L'ſ','s'}, {L'Ä','A'},{L'Ö','O'},{L'Ü','U'},{L'Ö','O'},{L'ß','S'},{L'É','E'},{L'È','E'},{L'Á','A'},{L'Ê','E'},{L'Â','A'}, {L'S','S'}};
  std::vector<wchar_t> add_e = {L'ä', L'ü', L'ö', L'Ä', L'Ü', L'Ö'};
  std::vector<wchar_t> add_s = {L'ß'};

  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring wide = converter.from_bytes(str); 
  std::string new_str;
  for(size_t i=0; i<wide.length(); i++) {
    if(rep.count(wide[i]) > 0) {
      new_str.push_back(rep[wide[i]]);
      if (std::find(add_e.begin(), add_e.end(), wide[i]) != add_e.end())
        new_str.push_back('e');
      else if (std::find(add_s.begin(), add_s.end(), wide[i]) != add_s.end())
        new_str.push_back('s');
    }
    else
      new_str.push_back((char)wide[i]);
  }
  return new_str;
}

std::string ConvertToId(std::string &name) {
  name = ReplaceNonUTF8(name);
  std::vector<std::string> words = Split(name, " ");
  std::string id = (char)std::tolower(words[0][0]) + words[0].substr(1);
  for (size_t i=1; i<words.size(); i++) {
    id += "_"; 
    id += (char)std::tolower(words[i][0]) + words[i].substr(1);
  }
  return id;
}

std::string ConvertFromId(std::string &id) {
  std::vector<std::string> words = Split(id, "_");
  std::string name = (char)std::toupper(words[0][0]) + words[0].substr(1);
  for (size_t i=1; i<words.size(); i++) {
    name += " "; 
    name += (char)std::toupper(words[i][0]) + words[i].substr(1);
  }
  return name;
}

nlohmann::json ValidateJson(std::string json_string, std::vector<std::string> keys) {
  nlohmann::json json;
  try {
    json = nlohmann::json::parse(json_string);
  } catch (std::exception& e) {
    return nlohmann::json();
  }
  for (auto key : keys) {
    nlohmann::json temp_json = json;
    for (auto temp_key : func::Split(key, "/")) {
      if (temp_json.count(temp_key) == 0)
        return nlohmann::json();
      temp_json = temp_json[temp_key];
    }
  }
  return json;
}

nlohmann::json Join(nlohmann::json &elem1, nlohmann::json &elem2) {
  for (auto it : elem2.items())
    elem1[it.key()] = it.value();
  return elem1;
}

std::string GetLastElemFromPath(std::string path) {
  return path.substr(path.rfind("/")+1);
}

std::string RemoveSequenzes(const std::filesystem::path& path, int sequenzes) {
  auto it = path.begin();
  std::advance(it, sequenzes);
  std::filesystem::path reduced; 
  for (; it!=path.end(); it++) {
    reduced /= *it;
  }
  return reduced;
}

} //Close namespace 
