#include "CText.hpp"
#include "CPlayer.hpp"

#define WHITE Webcmd::set_color(Webcmd::color::WHITE)

CText::CText(nlohmann::json jAttributes, CPlayer* p)
{
    m_player = p;
    for(auto atts : jAttributes)
        m_texts.push_back(new COutput(atts));
}

std::string CText::print()
{
    std::string sOutput = "";
    
    for(size_t i=0; i<m_texts.size(); i++)
        sOutput += m_texts[i]->print(m_player);

    if(sOutput != "")
        sOutput.erase(sOutput.end()-3,sOutput.end());
    return sOutput;
}

    
COutput::COutput(std::string sAtts)
{
    if(sAtts == "[]")
    {
        m_sSpeaker = "";
        m_sText = "";
        return;
    }

    std::vector<std::string> atts = func::split(sAtts, ";");
    m_sSpeaker = atts[0];
    m_sText = atts[1];
    if(atts.size()>2)
        m_jDeps = nlohmann::json::parse(atts[2]);
}

std::string COutput::getSpeaker() {
    return m_sSpeaker;
}
std::string COutput::getText() {
    return m_sText;
}
nlohmann::json COutput::getDeps() {
    return m_jDeps;
}

std::string COutput::print(CPlayer* p)
{
    if(m_jDeps.size() == 0)
        return m_sSpeaker + " " + m_sText + "\n";
    
    if(p->checkDependencies(m_jDeps) == false)
        return "";

    for(auto it=m_jDeps.begin(); it!=m_jDeps.end(); it++) {
        if(p->getMinds().count(it.key()) > 0)
            return p->getMinds()[it.key()].sColor + m_sSpeaker + " (level " + std::to_string(p->getMinds()[it.key()].level) + ": Erfolg) " + WHITE + m_sText + "\n";
    }
    return m_sSpeaker + " " + m_sText + "\n";
} 
