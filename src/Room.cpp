#include "CRoom.hpp" 
#include "CPerson.hpp"

// *** GETTER *** // 

string CRoom::getEntry() { 
    return m_sEntry; 
}

string CRoom::getArea() { 
    return m_sArea; 
}

CRoom::objectmap& CRoom::getCharacters() { 
    return m_characters; 
}

std::map<string, CExit*>& CRoom::getExtits() { 
    return m_exits; 
}

CRoom::objectmap CRoom::getExtits2() { 
    auto lambda = [](CExit* exit) { return exit->getName(); };
    return func::convertToObjectmap(m_exits, lambda); 
}

std::map<string, CItem*>& CRoom::getItems() { 
    return m_items; 
}

std::map<string, CDetail*>& CRoom::getDetails() { 
    return m_details; 
}

// *** SETTER *** //
void CRoom::setPlayers(objectmap& onlinePlayers) { m_players = onlinePlayers; }


// *** VARIOUS FUNCTIONS *** //

string CRoom::showDescription(std::map<std::string, CPerson*> mapCharacters)
{
    string sDesc = "><div class='spoken2'>";
    for(auto it : m_characters)
        sDesc += mapCharacters[it.first]->getRoomDescription();
    return m_text->print() + sDesc + "</div>";
}

string CRoom::showAll(std::string sMode)
{
    return showExits(sMode)+" "+showCharacters(sMode)+" "+showItems(sMode)+" "+showDetails(sMode)+"\n";
}

string CRoom::showExits(std::string sMode)
{
    if(sMode == "prosa")
    {
        auto lambda = [](CExit* exit) {return exit->getPrep() + " " + exit->getName();};
        std::string sProsa = func::printProsa(m_exits, lambda);
        if(sProsa != "")
            return "Ah, hier geht es " + sProsa;
        return "Der Raum hat keine Ausgänge.";
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
        if(func::printProsa(m_characters) != "")
            sOutput = "Hier sind " + func::printProsa(m_characters);
        if(func::printProsa(m_players) != "")
            sOutput += " Und außerdem noch "+ func::printProsa(m_players);
        if(sOutput == "")
            sOutput = "In diesem Raum sind keine Personen.";
    }
    else
        sOutput = "Characters: \n" + func::printList(m_characters) + func::printList(m_players);
    return sOutput;
}       

string CRoom::showItems(std::string sMode)
{
    string sOutput = "";
    auto printing = [](CItem* item) { return item->getName(); };
    auto condition = [](CItem* item) { return item->getHidden() != true; };

    if(sMode == "prosa")
    {
        std::string sProsa = func::printProsa(m_items, printing, condition);
        if(sProsa != "")
            return "Hier sind folgende Gegenstände: " + sProsa;
    }

    else
    {
        std::string sList = func::printList(m_items, printing, condition);
        if(sList != "")
            return "Items: \n" + sList;
    }
    return "Keine Gegenstände im Raum."; 
} 

string CRoom::showDetails(std::string sMode)
{
    std::string details = "";
    auto lamda = [](CDetail* detail) { return detail->getName(); };
    if(sMode == "prosa") 
        details += "Hier sind " + func::printProsa(m_details, lamda);
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
        if(detail.second->getLook() == sWhere && fuzzy::fuzzy_cmp(detail.second->getName(), sWhat) <= 0.2)
        {
            //Print output
            if(sMode == "prosa")
            {
                std::string sItems = func::printProsa(detail.second->getItems());
                if(sItems != "")
                    sOutput += "In " + detail.second->getName() + " sind " + sItems + ".\n";
                else
                    sOutput = "Nichts gefunden.\n";
            }
            else
                sOutput += detail.second->getName() + "\n" + func::printList(detail.second->getItems());
    
            //Change from hidden to visible and "empty" detail
            for(auto it : detail.second->getItems()) 
            {
                std::cout << it.first << std::endl;
                if(m_items.count(it.first) > 0)
                    m_items[it.first]->setHidden(false);
                else
                    std::cout << "Item not found: " << it.first << std::endl;
            }
            detail.second->getItems().clear();
        }
    }
    if(sMode=="list"  && sOutput == "") sOutput = "Nichts gefunden.\n";
    return sOutput;
}

CItem* CRoom::getItem(std::string sPlayerChoice)
{
    for(auto it : m_items)
    {
        if(it.second->getHidden() == true) 
            continue;
        if(fuzzy::fuzzy_cmp(it.second->getName(), sPlayerChoice) <= 0.2)
            return it.second;
    }
    return NULL;
}

