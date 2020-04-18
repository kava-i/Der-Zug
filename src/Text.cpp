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

    if(jAttributes.count("d_mind") > 0)
        m_jMinds = jAttributes["d_mind"];
    if(jAttributes.count("d_abilities") > 0)
        m_jAbillities = jAttributes["d_abilities"];
    if(jAttributes.count("u_mind") > 0)
        m_updateMind = jAttributes["u_mind"];
    if(jAttributes.count("u_abilities") > 0)
        m_updateAbilities = jAttributes["u_abilities"];
}

std::string COutput::getSpeaker() {
    return m_sSpeaker;
}
std::string COutput::getText() {
    return m_sText;
}

std::string COutput::print(CPlayer* p)
{
    //Variables
    std::string sSuccess = "";  //Storing, when success of mind will be announced 
    std::string sUpdated = "";  //Storing, when abilities/ minds are updated.

    //Check dependencies
    if(checkDependencies(sSuccess, p) == false)
        return ""; 
    
    //Update mind attributes
    updateAttrbutes(sUpdated, p);

    // *** Print text *** // 
    return p->returnSpeakerPrint(m_sSpeaker + sSuccess, m_sText + "\n" + sUpdated);
}

bool COutput::checkDependencies(std::string& sSuccess, CPlayer* p)
{
    //Check normal dependencies
    if(p->checkDependencies(m_jAbillities) == false)
    {
        std::cout << "dependencies failed.\n";
        return false; 
    }

    //Mind dependencies -> check if they match -> print with "success" || return nothing
    for(auto it=m_jMinds.begin(); it!=m_jMinds.end(); it++)
    {
        int val=it.value();
        std::cout << "Value: " << val << " (" << it.key() << std::endl;
        std::cout << "Level: " << p->getMind(it.key()).level << std::endl;
        if(p->getMind(it.key()).level >= val)
            sSuccess = DARK + " (level " + std::to_string(val) + ": Erfolg)" + WHITE;
        else
        {   
            std::cout << "mind failed.\n";
            return false; 
        }
    }
    return true;
}

void COutput::updateAttrbutes(std::string& sUpdated, CPlayer* p)
{
    //Check updates for minds
    for(auto it=m_updateMind.begin(); it!=m_updateMind.end(); it++)
    {
        int val=it.value();
        p->getMind(it.key()).level += val;
        sUpdated += p->getMind(it.key()).color + it.key()+ " updated!" + WHITE + "\n";
    }
    m_updateMind.clear();

    //Check updates for abilities
    for(auto it=m_updateAbilities.begin(); it!=m_updateAbilities.end(); it++)
    {
        int val=it.value();
        p->setStat(it.key(), p->getStat(it.key())+val);
        sUpdated += GREEN + it.key() + " updated!" + WHITE + "\n";
    } 
    m_updateAbilities.clear();
}
