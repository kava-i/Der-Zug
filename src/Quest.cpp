#include "CQuest.hpp"

CQuest::CQuest(nlohmann::json jAttributes)
{
    m_sName = jAttributes["name"];
    m_sID = jAttributes["id"];
    m_sDescription = jAttributes["description"];
    m_solved=false;
    m_active=false;
}

// *** GETTER *** //
std::string CQuest::getID() {
    return m_sID;
}
bool CQuest::getActive() {
    return m_active;
}

std::map<std::string, CQuestStep*> CQuest::getSteps() {
    return m_questSteps;
}

std::map<std::string, std::string> CQuest::getHandler() {
    return m_handler;
}

// *** SETTER *** //
void CQuest::setActive(bool active) {
    m_active = active;
}
void CQuest::setSteps(std::map<std::string, CQuestStep*> steps) {
    m_questSteps = steps;
}
void CQuest::setHandler(std::map<std::string, std::string> handlers) {
    m_handler = handlers;
}


// *** FUNCTIONS *** //
std::string CQuest::printQuest(bool solved)
{
    if(m_solved != solved)
        return "";
 
    std::string sOutput = "<b>" + m_sName + "</b>\n[<i>" + m_sDescription + "</i>]\n";
    size_t counter=1;
    for(auto it : m_questSteps)
    {
        if(it.second->getActive() == true)  {
            if(it.second->getSolved() == true)
                sOutput+=Webcmd::set_color(Webcmd::color::GREEN);
            sOutput += std::to_string(counter) + ". " + it.second->getName() + " [<i>" + it.second->getDescription() + "</i>]\n";
            counter++;
            sOutput+=Webcmd::set_color(Webcmd::color::WHITE);
        }
    }
    std::cout << sOutput << std::endl;
    return sOutput;    
}
 
void CQuest::checkSolved()
{
    for(auto it : m_questSteps) {
        if(it.second->getSolved() == false)
            return;
    }
    m_solved=true;
}

// ***** ***** CQUESTSTEP ***** ***** //

CQuestStep::CQuestStep(nlohmann::json jAttributes, CQuest* quest)
{
    m_sName = jAttributes["name"];
    m_sID = jAttributes["id"];
    m_sDescription = jAttributes["description"];
    m_solved = false;
    m_active = jAttributes.value("active", false);
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
bool CQuestStep::getSolved() {
    return m_solved;
}

// *** SETTER *** //
void CQuestStep::setActive(bool active) {
    m_active = active;
}


std::string CQuestStep::solved()
{
    m_solved = true;
    m_quest->checkSolved();
    if(m_active == true)
    {
        for(auto step : m_linkedSteps)
            m_quest->getSteps()[step]->setActive(true);
        return Webcmd::set_color(Webcmd::color::GREEN) + m_sName + " Erfolgreich!\n" + Webcmd::set_color(Webcmd::color::WHITE);
    }
    return "";
}
