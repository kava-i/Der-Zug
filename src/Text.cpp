#include "CText.hpp"
#include "CPlayer.hpp"

#define WHITE Webcmd::set_color(Webcmd::color::WHITE)
#define WHITEDARK Webcmd::set_color(Webcmd::color::WHITEDARK)
#define DARK Webcmd::set_color(Webcmd::color::DARK)
#define GREEN Webcmd::set_color(Webcmd::color::GREEN)

CText::CText(nlohmann::json jAttributes, CPlayer* p)
{
    m_player = p;

    for(auto atts : jAttributes) 
        m_texts.push_back(new COutput(atts, p));
}

std::string CText::print(bool events)
{
    std::string sOutput = "";
    
    for(size_t i=0; i<m_texts.size(); i++)
        sOutput += m_texts[i]->print(m_player, events);

    //if(sOutput != "")
    //    sOutput.pop_back();

    return sOutput;
}

std::string CText::reducedPrint(bool events)
{
    std::string sOutput = "";
    for(size_t i=0; i<m_texts.size(); i++)
        sOutput += m_texts[i]->reducedPrint(m_player, events);
    return sOutput;
}

    
COutput::COutput(nlohmann::json jAttributes, CPlayer* p)
{
    m_sSpeaker = jAttributes.value("speaker", "");
    m_sText = jAttributes.value("text", ""); 

    //Add dependencies and upgrades
    m_jDeps = jAttributes.value("deps", nlohmann::json::object());
    m_jUpdates = jAttributes.value("updates", nlohmann::json::object());

    //Add events
    m_pre_permanentEvents = jAttributes.value("pre_pEvents", std::vector<std::string>());
    m_pre_oneTimeEvents = jAttributes.value("pre_otEvents", std::vector<std::string>());
    m_post_permanentEvents = jAttributes.value("post_pEvents", std::vector<std::string>());
    m_post_oneTimeEvents = jAttributes.value("post_otEvents", std::vector<std::string>());
}

std::string COutput::getSpeaker() {
    return m_sSpeaker;
}
std::string COutput::getText() {
    return m_sText;
}

std::string COutput::print(CPlayer* p, bool events)
{
    //Variables
    std::string sSuccess = "";  //Storing, when success of mind will be announced 
    std::string sUpdated = "";  //Storing, when abilities/ minds are updated.

    //Check dependencies
    if(checkDependencies(sSuccess, p) == false)
        return ""; 
    
    //Update mind attributes
    updateAttrbutes(sUpdated, p);

    //Add events to players staged events
    if(events == true)
        addEvents(p);  

    //return text 
    return p->returnSpeakerPrint(m_sSpeaker + sSuccess, m_sText + "$" + sUpdated);
}

std::string COutput::reducedPrint(CPlayer* p, bool events)
{
    //Variables
    std::string sSuccess = "";  //Storing, when success of mind will be announced 

    //Check dependencies
    if(checkDependencies(sSuccess, p) == false)
        return ""; 

    //Add events to players staged events
    if(events==true)
        addEvents(p);  

    //Return text
    return m_sText;
}

bool COutput::checkDependencies(std::string& sSuccess, CPlayer* p)
{
    //Check dependencies
    if(p->checkDependencies(m_jDeps) == false)
        return false;
    
    //Create success output:
    for(auto it=m_jDeps.begin(); it!=m_jDeps.end(); it++)
    {
        if(p->getMinds().count(it.key()) > 0)
            sSuccess = DARK + " (level " + std::to_string(p->getMind(it.key()).level) + ": Erfolg)" + WHITE;
    }

    return true;
}

void COutput::updateAttrbutes(std::string& sUpdated, CPlayer* p)
{
    for(auto it=m_jUpdates.begin(); it!= m_jUpdates.end(); it++)
    {
        int val=it.value();

        //Check updates for minds
        if(p->getMinds().count(it.key()) > 0) {
            p->getMind(it.key()).level += val;
            sUpdated += p->getMind(it.key()).color + it.key()+ " erhöhrt!" + WHITE + "\n";
        }

        //Check updates for abilities
        else if(p->attributeExists(it.key()) == true) {
            p->setStat(it.key(), p->getStat(it.key())+val);
            sUpdated += GREEN + it.key() + " erhöht!" + WHITE + "\n";
        }
    }

    m_jUpdates.clear();
}

/**
* Add events to player staged events.
*/
void COutput::addEvents(CPlayer* p)
{
    for(const auto &it : m_pre_permanentEvents)
        p->addPreEvent(it);
    for(const auto &it : m_pre_oneTimeEvents)
        p->addPreEvent(it);
    m_pre_oneTimeEvents.clear();
    for(const auto &it : m_post_permanentEvents)
        p->addPostEvent(it);
    for(const auto &it : m_post_oneTimeEvents)
        p->addPostEvent(it);
    m_post_oneTimeEvents.clear(); 
}

