#include "CQuest.hpp"
#include "CPlayer.hpp"

CQuest::CQuest(nlohmann::json jAttributes)
{
    m_sName = jAttributes["name"];
    m_sID = jAttributes["id"];
    m_sDescription = jAttributes["description"];
    m_EP = jAttributes.value("ep", 5);
    m_solved=false;
    m_active=false;
    m_onlineFromBeginning = jAttributes.value("online", "true") == "true";
}

// *** GETTER *** //
std::string CQuest::getID() {
    return m_sID;
}
bool CQuest::getSolved() {
    return m_solved;
}
bool CQuest::getActive() {
    return m_active;
}
bool CQuest::getOnlineFromBeginning() {
    return m_onlineFromBeginning;
}

std::map<std::string, CQuestStep*> CQuest::getSteps() {
    return m_questSteps;
}

std::vector<nlohmann::json> CQuest::getHandler() {
    return m_handler;
}


// *** SETTER *** //
void CQuest::setSteps(std::map<std::string, CQuestStep*> steps) {
    m_questSteps = steps;
}

void CQuest::setHandler(std::vector<nlohmann::json> handlers) {
    m_handler = handlers;
}


// *** FUNCTIONS *** //

std::string CQuest::setActive(int& ep, CPlayer* p) {
    m_active = true;
    m_questSteps.begin()->second->setActive(true, p);

    std::string sOutput = "Neue Quest: <b>"+m_sName + "</b>: <i>"+m_sDescription+"</i>\n";

    for(auto it : m_questSteps)
    {
        if(it.second->getSolved() == true)
            sOutput += it.second->handleSolved(p);
    }
    sOutput += checkSolved(ep);
    return sOutput;
}


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

            sOutput += std::to_string(counter) + ". " + it.second->getName() + " [<i>" + it.second->getDescription() + "</i>]";
            
            if(it.second->getSucc() > 0)
                sOutput += " [" + std::to_string(it.second->getCurSucc()) + "/" + std::to_string(it.second->getSucc()) + "]";

            counter++;
            sOutput += "\n" + Webcmd::set_color(Webcmd::color::WHITE);
        }
    }
    std::cout << sOutput << std::endl;
    return sOutput;    
}
 
std::string CQuest::checkSolved(int& ep)
{
    for(auto it : m_questSteps) {
        if(it.second->getSolved() == false)
            return "";
    }
    m_solved=true;
    if(m_active == true)
    {
        ep = m_EP;
        return "Quest "+ m_sName + " abgeschlossen! + " + std::to_string(m_EP) + " EP\n";
    }
    return "";
}

// ***** ***** CQUESTSTEP ***** ***** //

CQuestStep::CQuestStep(nlohmann::json jAttributes, CQuest* quest)
{
    m_sName = jAttributes["name"];
    m_sID = jAttributes["id"];
    m_sDescription = jAttributes["description"];
    m_solved = false;
    m_active = false;

    m_succ = jAttributes.value("list", 0);
    m_curSucc = 0;
   
    m_events = jAttributes.value("events", "");
    m_preEvents = jAttributes.value("_events", "");
    m_info = jAttributes.value("info", std::map<std::string, std::string>());
    
    std::vector<std::string> linkedSteps;
    if(jAttributes.count("linkedSteps") != 0)
        linkedSteps = jAttributes["linkedSteps"].get<std::vector<std::string>>();
    m_linkedSteps = linkedSteps;
    m_quest = quest;
}


// *** GETTER *** //
std::string CQuestStep::getID() {
    return m_sID;
}
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
int CQuestStep::getSucc() {
    return m_succ;
}
int CQuestStep::getCurSucc() {
    return m_curSucc;
}
std::vector<std::string>& CQuestStep::getWhich() {
    return m_which;
}
std::string CQuestStep::getEvents() {
    return m_events;
}
std::string CQuestStep::getPreEvents() {
    return m_preEvents;
}
std::map<std::string, std::string> CQuestStep::getInfo() {
    return m_info;
}

// *** SETTER *** //
void CQuestStep::setActive(bool active, CPlayer* p) {
    m_active = active;
    if(m_solved==true)
        handle_events(p);
}
void CQuestStep::setCurSucc(int x) {
    m_curSucc = x;
}
void CQuestStep::incSucc(int x) {
    m_curSucc += x;
}

std::string CQuestStep::solved(int& ep, CPlayer* p)
{
    std::string sOutput = "";
    if(m_succ != m_curSucc)
        return sOutput;

    m_solved = true;
    if(m_active == true)
    {
        sOutput += handleSolved(p);
        handle_events(p);
    }
    sOutput += m_quest->checkSolved(ep);

    return sOutput;
}

std::string CQuestStep::handleSolved(CPlayer* p)
{
    for(auto step : m_linkedSteps)
        m_quest->getSteps()[step]->setActive(true, p);
    return m_sName + " Erfolgreich!\n";
}

void CQuestStep::handle_events(CPlayer* p)
{
    //Add events triggered after next output.
    p->addPostEvent(m_events);

    //Check if player command shall not be executed.
    if(m_info.count("break") > 0)
        p->getContext(m_quest->getID())->setCurPermeable(false);

    //Check for event to directly trigger.
    if(m_preEvents != "")
        p->throw_events(m_preEvents, "quest/ step: " + m_quest->getID() + "/" + m_sID);
}
