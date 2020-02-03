#include "CQuest.hpp"

CQuest::CQuest(nlohmann::json jAttributes)
{
    m_sName = jAttributes["name"];
    m_sID = jAttributes["id"];
    m_sDescription = jAttributes["description"];
    m_solved=false;
    m_active=true;
}

// *** GETTER *** //
bool CQuest::getActive() {
    return m_active;
}

std::map<std::string, CQuestStep*> CQuest::getSteps() {
    return m_questSteps;
}

// *** SETTER *** //
void CQuest::setSteps(std::map<std::string, CQuestStep*> steps) {
    m_questSteps = steps;
}

std::string CQuest::printQuest()
{
    std::string sOutput = "<b>" + m_sName + "</b>\n[<i>" + m_sDescription + "</i>]\n";
    size_t counter=1;
    for(auto it : m_questSteps)
    {
        if(it.second->getActive() == true)  {
            sOutput += std::to_string(counter) + ". " + it.second->getName() + " [<i>" + it.second->getDescription() + "</i>]\n";
            counter++;
        }
    }
    std::cout << sOutput << std::endl;
    return sOutput;    
}
 

// ***** ***** CQUESTSTEP ***** ***** //

CQuestStep::CQuestStep(nlohmann::json jAttributes, CQuest* quest)
{
    m_sName = jAttributes["name"];
    m_sID = jAttributes["id"];
    m_sDescription = jAttributes["description"];
    m_solved = true;
    m_active = true;
    std::vector<std::string> linkedSteps;
    if(jAttributes.count("linkedSteps") != 0)
        linkedSteps = jAttributes["linkedSteps"].get<std::vector<std::string>>();
    m_linkedSteps = linkedSteps;
    m_quest = quest;
}


// *** GETTER *** //
std::string CQuestStep::getName() {
    return m_sName;
}
std::string CQuestStep::getDescription() {
    return m_sDescription;
}
bool CQuestStep::getActive() {
    return m_active;
}

// *** SETTER *** //
void CQuestStep::setActive(bool active) {
    m_active = active;
}


void CQuestStep::solved()
{
    m_solved = true;
    if(m_active == true)
    {
        for(auto step : m_linkedSteps)
            m_quest->getSteps()[step]->setActive(true);
    }
}
