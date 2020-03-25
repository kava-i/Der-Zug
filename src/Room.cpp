#include "CRoom.hpp" 

// *** GETTER *** // 

string CRoom::getDescription()  { return m_text->print(); }
string CRoom::getEntry()        { return m_sEntry; }
string CRoom::getArea()         { return m_sArea; }
CRoom::objectmap& CRoom::getCharacters()    { return m_characters; }
CRoom::objectmap& CRoom::getExtits2()       { return m_exits_objectMap; }
std::map<string, CExit*>& CRoom::getExtits() { return m_exits; }
std::map<string, CItem*>& CRoom::getItems()  { return m_items; }

// *** SETTER *** //
void CRoom::setPlayers(objectmap& onlinePlayers) { m_players = onlinePlayers; }


// *** VARIOUS FUNCTIONS *** //

string CRoom::showEntryDescription(std::map<std::string, CCharacter*>& mapChars)
{
    string sDesc = m_sEntry + m_text->print() + "\n";
    for(auto it : m_characters)
        sDesc.append(mapChars[it.first]->getDescription());
    return sDesc;
}

string CRoom::showDescription(std::map<std::string, CCharacter*>& mapChars)
{
    string sDesc = m_text->print();
    for(auto it : m_characters) 
        sDesc.append(mapChars[it.first]->getDescription());
    return sDesc;
}

string CRoom::showAll(std::string sMode)
{
    string sDesc = m_text->print()+ "\n";
    sDesc+=showDetails(sMode);
    sDesc+=showExits(sMode);
    sDesc+=showCharacters(sMode);
    sDesc+=showItems(sMode);
    return sDesc;
}

string CRoom::showExits(std::string sMode)
{
    if(sMode == "prosa")
    {
        auto lamda = [](CExit* exit) {return exit->getPrep() + " " + exit->getName();};
        return "PERZEPTION - Ah, hier geht es " + func::printProsa(m_exits,lamda) + ".\n";
    }
    else
    {
        auto lamda = [](CExit* exit) {return exit->getName(); };
        return "Exits: \n" + func::printList(m_exits, lamda);
    }
}

string CRoom::showCharacters(std::string sMode)
{
    std::string sOutput = "";
    if(sMode == "prosa")
    {
        sOutput = "PERZEPTION - Hier sind " + func::printProsa(m_characters) + "\n";
        if(func::printProsa(m_players) != "")
            sOutput += "Und außerdem noch "+ func::printProsa(m_players) + "\n";
    }
    else
        sOutput = "Characters: \n" + func::printList(m_characters) + func::printList(m_players);
    return sOutput;
}       

string CRoom::showItems(std::string sMode)
{
    string sOutput = "";
    auto printing = [](CItem* item) { return item->getName() + " (" + item->getDescription() + ")"; };
    auto condition = [](CItem* item) { return item->getAttribute<bool>("hidden") != true; };

    if(sMode == "prosa")
        return "PERZEPTION - Ah, hier sind folgende Gegenstände: " + func::printProsa(m_items, printing, condition);
    else
        return "Items: \n" + func::printList(m_items, printing, condition);
} 

string CRoom::showDetails(std::string sMode)
{
    std::string details = "";
    auto lamda = [](CDetail* detail) { return detail->getDescription(); };
    if(sMode == "prosa") 
        details += "PERZEPTION - " + func::printProsa(m_details, lamda) + "\n";
    else 
        details += "Details:\n" + func::printList(m_details, lamda);

    if(details == "")
        details = "Keine Details zu finden.\n";
    return details;
}

string CRoom::look(string sWhere, string sWhat, std::string sMode)
{
    string sOutput = "";
    for(auto detail : m_details)
    {
        std::cout << "Detail: " << detail.first << std::endl;

        if(detail.second->getLook() == sWhere && fuzzy::fuzzy_cmp(detail.second->getName(), sWhat) <= 0.2)
        {
            //Print output
            if(sMode == "prosa")
                sOutput += "PERZEPTION - In " + detail.second->getName() + " sind " + func::printProsa(detail.second->getItems()) + ".\n";
            else
                sOutput += detail.second->getName() + "\n" + func::printList(detail.second->getItems());
    
            //Change from hidden to visible and "empty" detail
            for(auto it : detail.second->getItems()) 
                m_items[it.first]->setAttribute<bool>("hidden", false);
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

