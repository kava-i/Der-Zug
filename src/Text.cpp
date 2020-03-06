#include "CText.hpp"
#include "CPlayer.hpp"

#define WHITE Webcmd::set_color(Webcmd::color::WHITE)

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
        sOutput += m_texts[i]->print(m_player);

   // if(sOutput != "")
   //     sOutput.erase(sOutput.end()-3,sOutput.end());
    return sOutput;
}

    
COutput::COutput(std::string sAtts, CPlayer* p)
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
    if(atts.size()>3) {
        nlohmann::json j = nlohmann::json::parse(atts[3]);
        mUpdates = j.get<std::map<std::string, int>>();
    }

    if(p == NULL)
        return;

    //check for "_" in speaker -> speaker is mind and has a depenency
    std::vector<std::string> mind_val = func::split(m_sSpeaker, "_");
    if(mind_val.size() > 1)
    {
        m_sSpeaker = mind_val[0];
        m_mind = std::make_pair(func::returnToLower(m_sSpeaker), (int)mind_val[1][0] - 48);
    }
    //Check if speaker is mind
    else if(p->getMinds().count(func::returnToLower(m_sSpeaker)) > 0)
        m_mind = std::make_pair(func::returnToLower(m_sSpeaker), 0);
    else
        m_mind = std::make_pair("", 0);
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
    //Update mind attributes
    std::string sOutput = "";
    for(auto it=mUpdates.begin(); it!=mUpdates.end(); it++) {
        p->getMinds()[it->first].level += it->second;
        sOutput += p->getMinds()[it->first].color + it->first + " updated!\n" + WHITE;
    }
    mUpdates.clear();

    // *** Print text *** // 

    //No dependencies -> simple print
    if(m_jDeps.size() == 0 && m_mind.first=="")
        return m_sSpeaker + " " + m_sText + "\n" + sOutput;
    
    //Normal dependencies don't match -> return nothing
    if(p->checkDependencies(m_jDeps) == false)
        return "";

    //Mind dependencies -> check if they math -> print with "success" || return nothing
    if(m_mind.second != 0) {
        if(p->getMinds()[m_mind.first].level >= m_mind.second)
            return "\n" + p->getMinds()[m_mind.first].color + m_sSpeaker + " (level " + std::to_string(m_mind.second) + ": Erfolg) " + WHITE + m_sText + "\n" + sOutput;
        else
            return "";
    }

    return m_sSpeaker + " " + m_sText + "\n" + sOutput;
}
