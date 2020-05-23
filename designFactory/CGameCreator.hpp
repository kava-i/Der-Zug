#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>
#include <string>
#include "json.hpp"
#include "func.hpp"

//using namespace nlohmann;

class CGameCreator
{
    std::string m_pathToWorld;
    std::map<std::string, std::string> m_mapAttributes;
    
public:
    CGameCreator();
    void selectWorld();
    void editCharacter();
    void editDialog();

    //Helping functions
    nlohmann::json readJson(std::string sPath);
    std::string select(std::string sPath);
    nlohmann::json selectFromJson(nlohmann::json j);
    std::string selectAttribute(nlohmann::json j);

    void showAttributes(nlohmann::json j);
    std::string print(nlohmann::json);


    bool editString(std::string sKey, std::string sValue, nlohmann::json&, nlohmann::json);
    bool editDesc(std::string sKey, nlohmann::json&, nlohmann::json);
    bool editList(std::string sKey, nlohmann::json& file, nlohmann::json object, bool du=false);
    void deps(nlohmann::json& element);
    void updates(nlohmann::json& element);
    void events(nlohmann::json& element);

    nlohmann::json createDesc();
    void write(nlohmann::json, std::string sPath);
    bool yes();
}; 
