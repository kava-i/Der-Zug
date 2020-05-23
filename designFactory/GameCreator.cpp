#include "CGameCreator.hpp"

namespace fs = std::filesystem;

CGameCreator::CGameCreator()
{
    m_mapAttributes["name"] = "string";
    m_mapAttributes["id"] = "string";

    m_mapAttributes["roomDescription"] = "desc";
    m_mapAttributes["description"] = "desc";
    m_mapAttributes["text"] = "desc";

    m_mapAttributes["exits"] = "list";
    m_mapAttributes["characters"] = "list";
    m_mapAttributes["items"] = "list";
    m_mapAttributes["details"] = "list";
    m_mapAttributes["options"] = "list";
}

void CGameCreator::selectWorld()
{
    std::cout << "Which world do you want to design?\n";
    m_pathToWorld = select("../src/factory")+"/jsons";
} 


void CGameCreator::editCharacter()
{
    //Select section
    std::cout << "Choose section: \n";
    std::string sPath = select(m_pathToWorld + "/characters");
    nlohmann::json section = readJson(sPath);

    //Select character
    std::cout << "Select character: \n";
    nlohmann::json character = selectFromJson(section);
    if(character.size() == 0)
        return;

    for(;;)
    {
        std::cout << character["name"] << "'s attributes:\n";

        //Show add select characters attributes
        std::string sAttribute = selectAttribute(character);
        if(sAttribute == "")
            continue;
        
        //Edit value
        if(m_mapAttributes[sAttribute] == "string")
        {
            if(editString(sAttribute, character[sAttribute], section, character) == false)
                std::cout << "Nothing changed.\n";
            else
                write(section, sPath);
        }
        else if(m_mapAttributes[sAttribute] == "desc")
        {
            if(editDesc(sAttribute, section, character) == false)
                std::cout << "Nothing changed.\n";
            else
                write(section, sPath);
        }
    
        break;
    }
}

void CGameCreator::editDialog()
{
    //Select dialog
    std::cout << "Choose dialog: \n";
    std::string sPath = select(m_pathToWorld + "/dialogs");
    nlohmann::json dialog = readJson(sPath);

    //Select state to edit
    std::cout << "Select state to edit (enter \"new\" to create new state): \n";
    nlohmann::json state = selectFromJson(dialog);

    std::string sAttribute = "";
    while(sAttribute == "")
    {
        //Show attributes
        sAttribute = selectAttribute(state);
        if(sAttribute == "q")
            return;
    }

    if(m_mapAttributes[sAttribute] == "desc")
    {
        if(editDesc(sAttribute, dialog, state) == false)
            std::cout << "Nothing changed.\n";
        else
            write(dialog, sPath);
    }
    else if(m_mapAttributes[sAttribute] == "list")
    {
        std::cout << "Until \"q\" is entered, enter a new element in the form: \"{\"id\":1, \"text\":\"Wer bist du?\", \"to\":\"ich_bins\"}\"" << std::endl;
        if(editList(sAttribute, dialog, state, true) == false)
            std::cout << "Nothing changed.\n";
        else
            write(dialog, sPath);

    }
}

// *** HELP FUNCTIONS *** //
nlohmann::json CGameCreator::readJson(std::string sPath)
{
    std::ifstream read(sPath);
    nlohmann::json j;
    read >> j;
    read.close();
    return j;
}

std::string CGameCreator::select(std::string sPath)
{
    std::map<int, std::string> paths;
    int counter=1;
    for(auto& p : fs::directory_iterator(sPath))
    {
        std::cout << counter << ": " << p.path().stem()<< std::endl;
        paths[counter] = p.path();
        counter++;
    }
    std::cout << ">";
    int input;
    std::cin >> input; 
    return paths[input];
}

std::string CGameCreator::selectAttribute(nlohmann::json object)
{
    //Show add select characters attributes
    showAttributes(object);
    std::cout << "Which attribute do you want to edit?\n> ";
    std::string sAttribute;
    std::cin.ignore();
    getline(std::cin, sAttribute);

    if(sAttribute == "q")
        return "q";

    if(object.count(sAttribute) == 0)
    {
        std::cout << "Attribute does not exist. Look out for spelling mistakes.\n";
        return "";
    }
    return sAttribute;
}
nlohmann::json CGameCreator::selectFromJson(nlohmann::json j)
{
    std::map<int, nlohmann::json> mapObjects;
    int counter = 1;
    for(const auto& it : j)
    {
        mapObjects[counter] = it;
        std::cout << counter << ": " << it["id"] << std::endl;
        counter++;
    }
    std::cout << "> ";
    int input;
    std::cin >> input;
    if(mapObjects.count(input) == 0)
    {
        std::cout << "Does not exits!\n";
        return nlohmann::json::object();
    }
    return mapObjects[input];
}

void CGameCreator::showAttributes(nlohmann::json j)
{
    for(auto it = j.begin(); it != j.end(); it++)
        std::cout << it.key() << ": " << print(it.value()) << std::endl;
}

std::string CGameCreator::print(nlohmann::json j)
{
    std::string str = "";
    if(j.type() == nlohmann::json::value_t::array)
    {
        for(const auto& it : j)
            str += "\n- " + it.dump();
    }
    else
        str = j;
    return str;
}

bool CGameCreator::editString(std::string sKey, std::string sValue, nlohmann::json& file, nlohmann::json object)
{
    //variables
    std::string newValue;

    std::cout << "Enter new value instead of: \"" << sValue << "\":\n";
    getline(std::cin, newValue);
    std::cout << "Happy with your changes: \n";
    std::cout << sKey << ": \"" << sValue << "\" becomes: \"" << newValue << "\"? (y/n)\n";
    if(yes() == false)
        return false;

    for(auto& it : file)
    {
        if(it["id"] == object["id"])
            it[sKey] = newValue;
    }
    return true;
}

bool CGameCreator::editDesc(std::string sKey, nlohmann::json& file, nlohmann::json object)
{
    int num=1;
    std::cout << "There are " << object[sKey].size() << " elements. Which one do you want to edit? Enter a number greater that the number of elements to add a new description\n";
    std::cin>>num;
    std::cin.ignore();

    std::cout << "Delete (1), change all/ new element (2): "; 
    int choice;
    std::cin >> choice;
    std::cin.ignore();

    //Erase
    if(choice == 1)
    {
        for(auto& it : file)
        {
            if(it["id"] == object["id"])
                it[sKey].erase(num-1);
        }
        return true;
    }

    //Change all
    else if(choice == 2)
    {
        nlohmann::json text = createDesc(); 
        std::cout << text << std::endl;

        std::cout << "Happy with your changes? (y/n)\n";
        if(yes() == false)
            return false; 
     
        for(auto& it : file)
        {
            if(it["id"] == object["id"])
            {
                if(num > it[sKey].size())
                    it[sKey].push_back(text);
                else
                {
                    int i=0;
                    for(auto& jt : it[sKey]) {
                        if(i==num-1)
                            jt = text;
                        i++;
                    }
                }
            }
        }
        return true;
    }

    else
        return false;   
}

nlohmann::json CGameCreator::createDesc()
{
    std::string sSpeaker;
    std::string sText;
    nlohmann::json text;

    std::cout << "Speaker: ";
    getline(std::cin, sSpeaker);
    
    std::cout << "Text: ";
    getline(std::cin, sText);

    text["speaker"] = sSpeaker;
    text["text"] = sText;

    deps(text);
    updates(text);
    events(text);

    return text;
}

bool CGameCreator::editList(std::string sKey, nlohmann::json& file, nlohmann::json object, bool du)
{
    std::string sInput = "";
    nlohmann::json element;
    for(;;)
    {
        nlohmann::json j;
        std::cout << "> ";
        getline(std::cin, sInput);
        if(sInput == "q") break;
        
        j=nlohmann::json::parse(sInput);
        if(du== true) {
            deps(j);
            updates(j);
        }
        element.push_back(j);
    }
    for(auto& it : file)
    {
        if(it["id"] == object["id"])
            it[sKey].push_back(element);
    }
}

void CGameCreator::deps(nlohmann::json& element)
{
    std::cout << "dependencies? (y/n)\n";
    if(yes() == true)
    {
        std::string sDeps;
        std::cout << "Enter for example: {\"gold\": \">9\", \"logik\":\">0\"}\n> ";
        getline(std::cin, sDeps);
        element["deps"] = nlohmann::json::parse(sDeps);
    }
}

void CGameCreator::updates(nlohmann::json& element)
{
    std::cout << "updates? (y/n)\n";
    if(yes() == true)
    {
        std::string sUpdates;
        std::cout << "Enter for example: {\"logik\":\"1\", \"highness\": \"2\"}\n> ";
        getline(std::cin, sUpdates);
        element["updates"] = nlohmann::json::parse(sUpdates);
    }
}

void CGameCreator::events(nlohmann::json& element)
{
    std::cout << "events? (y/n)\n";
    if(yes() == true)
    {
        std::string sEvents;
        std::cout << "You can choose between \"pre_pEvents\", \"pre_otEvents\", \"post_pEvents\" or \"post_otEvents\"\n";
        std::cout << "Enter for example: post_otEvents:[\"talk taxifahrer\"]\n";
        getline(std::cin, sEvents);
        element[func::split(sEvents, ":")[0]] = nlohmann::json::parse(func::split(sEvents, ":")[1]);
    }
}
void CGameCreator::write(nlohmann::json j, std::string sPath)
{
    std::ofstream write(sPath);
    write << j;
    write.close();
    std::cout << "Changes writen to disk (" << sPath << ")\n";
}

bool CGameCreator::yes()
{
    std::string input;
    getline(std::cin, input);
    return input == "y";
}
    
    
        
