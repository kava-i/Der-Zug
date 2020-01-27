#include "CWorldContext.hpp"
#include "CPlayer.hpp"


// ***** PARSER ***** //

vector<CContext::event> CWorldContext::parser(string sInput, CPlayer* p)
{
    std::cout << "worldParser: " << sInput << std::endl;
    std::regex deleteChar("(deleteCharacter)_(.*)");
    std::regex addItem("(addItem)_(.*)");
    std::regex newFight("(fight)_(.*)");
    std::regex endFight("endFight");
    std::regex endDialog("endDialog");
    std::smatch m;

    vector<string> commands = func::split(sInput, "/");
    vector<event> events;

    for(size_t i=0; i<commands.size(); i++)
    { 
        if(std::regex_search(commands[i], m, deleteChar))
            events.push_back(std::make_pair("deleteCharacter", m[2]));
        else if(std::regex_search(commands[i], m, addItem))
            events.push_back(std::make_pair("addItem", m[2]));
        else if(std::regex_match(commands[i], m, newFight))
            events.push_back(std::make_pair("newFight", m[2]));
        else if(std::regex_match(commands[i], endFight))
            events.push_back(std::make_pair("endFight", ""));
        else if(std::regex_match(commands[i], endDialog))
            events.push_back(std::make_pair("endDialog", ""));
    }

    if(events.size()==0)
    {   
        m_permeable = true;
        return {};
    }

    return events;
}

// **** HANLDERS ***** //

void CWorldContext::h_deleteCharacter(string& sIdentifier, CPlayer* p) {
    p->getRoom()->getCharacters().erase(sIdentifier);
    delete p->getWorld()->getCharacters()[sIdentifier];
    p->getWorld()->getCharacters().erase(sIdentifier); 

    m_permeable=false;
}

void CWorldContext::h_addItem(string& sIdentifier, CPlayer* p) {
    p->addItem(p->getWorld()->getItem(sIdentifier));
    m_permeable=false;
}

void CWorldContext::h_endFight(string& sIdentifier, CPlayer* p) {
    p->endFight();
    m_permeable=false;
}

void CWorldContext::h_endDialog(string& sIdentifier, CPlayer* p) {
    p->getContexts().erase("dialog");
    m_permeable=false;
}


void CWorldContext::h_newFight(string& sIdentifier, CPlayer* p) {
    p->setFight(new CFight(p, p->getWorld()->getCharacters()[sIdentifier]));
}
