#include "CRoom.hpp" 

CRoom::CRoom(string sName, string sID, string sDescription, string sEntry, objectmap exits, objectmap characters, std::map<string, CItem*> items, std::map<string, CDetail*> details)
{
    m_sName = sName;
    m_sID = sID;
    m_sDescription = sDescription;
    m_sEntry = sEntry;
    m_exists = exits;
    m_characters = characters;
    m_items = items;
    m_details = details;
}


// *** GETTER *** // 

string CRoom::getName()         { return m_sName; }
string CRoom::getID()           { return m_sID; }
string CRoom::getDescription()  { return m_sDescription; }
string CRoom::getEntry()        { return m_sEntry; }
CRoom::objectmap& CRoom::getExtits()        { return m_exists; }
CRoom::objectmap& CRoom::getCharacters()    { return m_characters; }
std::map<string, CItem*>& CRoom::getItems()  { return m_items; }

// *** SETTER *** //
void CRoom::setPlayers(objectmap& onlinePlayers) { m_players = onlinePlayers; }


// *** VARIOUS FUNCTIONS *** //

string CRoom::showEntryDescription(std::map<std::string, CCharacter*>& mapChars)
{
    string sDesc = m_sEntry + m_sDescription + "\n";
    for(auto it : m_characters)
        sDesc.append(mapChars[it.first]->getDescription());

    return sDesc;
}

string CRoom::showDescription(std::map<std::string, CCharacter*>& mapChars)
{
    string sDesc = m_sDescription;
    for(auto it : m_characters) 
        sDesc.append("\n" + mapChars[it.first]->getDescription());
    return sDesc;
}

string CRoom::showAll()
{
    string sDesc = m_sDescription + "\n";
    sDesc+=showDetails();
    sDesc+=showExits();
    sDesc+=showCharacters();
    sDesc+=showItems();
    return sDesc;
}

string CRoom::showExits()
{
    string exits = "Exits: \n";
    size_t counter=1;
    for(auto it : m_exists) {
        exits += std::to_string(counter) + ": " + it.second + "\n";
        counter++;
    }
    return exits;
}

string CRoom::showCharacters()
{
    string characters = "Characters: \n";
    size_t counter=1;
    for(auto it : m_characters) {
        characters += std::to_string(counter) + ": " + it.second + "\n";
        counter++;
    }
    for(auto it : m_players) {
        if(it.second == m_sID)
        {
            characters += std::to_string(counter) + ": " + it.first + "\n";
            counter++;
        }
    }
    return characters;
}       

string CRoom::showItems()
{
    string items = "Items: \n";
    size_t counter=1;
    for(auto it : m_items) {
        if(it.second->getAttribute<bool>("hidden") == true) continue;
        items += std::to_string(counter) + ": " + it.second->getAttribute<string>("name")+" - "+it.second->getAttribute<string>("description") + "\n";
        counter++;
    }
    return items;
} 

string CRoom::showDetails()
{
    string details;
    for(auto it : m_details)
        details += it.second->getDescription() + "\n";
    return details;
}

string CRoom::look(string sWhere, string sWhat)
{
    string sOutput = "";
    for(auto detail : m_details)
    {
        std::cout << "Detail: " << detail.first << std::endl;
        if(detail.second->getLook() == sWhere && fuzzy::fuzzy_cmp(detail.second->getName(), sWhat) <= 0.2)
        {
            size_t counter = 1;
            size_t numItems = detail.second->getItems().size();
            if(numItems == 0) {
                sOutput += detail.second->getName() + " is empty. \n";
                continue;
            }


            sOutput += "You found ";
            for(auto it : detail.second->getItems()) {
                
                sOutput += "a " + it.second + " (" + m_items[it.first]->getAttribute<string>("description") + ")";
                if(counter == numItems-1) 
                    sOutput+= " and ";
                else if( counter < numItems-1)
                    sOutput += ", ";
                counter++;
                m_items[it.first]->setAttribute<bool>("hidden", false);
            }
            sOutput += " in a " + detail.second->getName() + ".\n";
            detail.second->getItems().clear();
        }
    }
    return sOutput;
}

CItem* CRoom::getItem(std::string sPlayerChoice)
{
    for(auto it : m_items)
    {
        if(it.second->getAttribute<bool>("hidden") == true) 
            continue;
        if(fuzzy::fuzzy_cmp(it.second->getAttribute<string>("name"), sPlayerChoice) <= 0.2)
            return it.second;
    }
    return NULL;
}

