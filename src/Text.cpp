#include "CText.hpp"
#include "CPlayer.hpp"

#define WHITE Webcmd::set_color(Webcmd::color::WHITE)
#define WHITEDARK Webcmd::set_color(Webcmd::color::WHITEDARK)
#define DARK Webcmd::set_color(Webcmd::color::DARK)

CText::CText(nlohmann::json jAttributes, CPlayer* p)
{
    m_player = p;

    for(auto atts : jAttributes) 
        m_texts.push_back(new COutput(atts, p));
}

std::string CText::print()
{
    std::string sOutput = "";
    
    for(size_t i=0; i<m_texts.size(); i++)
        sOutput += func::returnSwapedString(m_texts[i]->print(m_player), m_player->getStat("highness"));

    if(sOutput != "")
        sOutput.pop_back();

    return sOutput;
}

    
COutput::COutput(nlohmann::json jAttributes, CPlayer* p)
{
    m_sSpeaker = jAttributes.value("speaker", "");
    m_sText = jAttributes.value("text", "") + "$";
    m_mind = jAttributes.value("mind", -1);

    if(jAttributes.count("deps") > 0)
        m_jAbillities = jAttributes["abilities"];
    if(jAttributes.count("update") > 0)
        mUpdates = jAttributes["update"].get<std::map<std::string, std::string>>();
}

std::string COutput::getSpeaker() {
    return m_sSpeaker;
}
std::string COutput::getText() {
    return m_sText;
}

std::string COutput::print(CPlayer* p)
{
    std::string sUpdated = "";
    std::string sSuccess = "";

    //Check normal dependencies
    if(p->checkDependencies(m_jAbillities) == false)
        return "";

    //Mind dependencies -> check if they match -> print with "success" || return nothing
    if(m_mind != -1) {
        if(p->getMinds()[func::returnToLower(m_sSpeaker)].level >= m_mind)
            sSuccess = DARK + " (level " + std::to_string(m_mind) + ": Erfolg)" + WHITE;
        else
            return "";
    }
    
    //Update mind attributes
    /*
    std::string sOutput = "";
    for(auto it=mUpdates.begin(); it!=mUpdates.end(); it++) {
        
        p->getMinds()[it->first].level += it->second;
        sOutput += p->getMinds()[it->first].color + it->first + " updated!" + WHITE + "\n";
    }
    mUpdates.clear();
    */

    // *** Print text *** // 
    return "<div class='spoken'>" + m_sSpeaker + sSuccess + " - " + WHITEDARK + m_sText + WHITE + sUpdated + "</div>";
}
