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
        sOutput += m_texts[i]->print(m_player);

    if(sOutput != "")
        sOutput.pop_back();

    return sOutput;
}

std::string CText::reducedPrint()
{
    std::string sOutput = "";
    for(size_t i=0; i<m_texts.size(); i++)
        sOutput += m_texts[i]->reducedPrint(m_player);
    return sOutput;
}

    
COutput::COutput(nlohmann::json jAttributes, CPlayer* p)
{
    m_sSpeaker = jAttributes.value("speaker", "");
    m_sText = jAttributes.value("text", ""); 

    if(jAttributes.count("deps") > 0)
        m_jDeps = jAttributes["deps"];
    if(jAttributes.count("updates") > 0)
        m_jUpdates = jAttributes["updates"];
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
    return p->returnSpeakerPrint(m_sSpeaker + sSuccess, func::returnSwapedString(m_sText, p->getStat("highness")) + "$\n" + sUpdated);
}

std::string COutput::reducedPrint(CPlayer* p)
{
    //Variables
    std::string sSuccess = "";  //Storing, when success of mind will be announced 

    //Check dependencies
    if(checkDependencies(sSuccess, p) == false)
        return ""; 

    // *** Print text *** // 
    return func::returnSwapedString(m_sText, p->getStat("highness"));
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
            sUpdated += p->getMind(it.key()).color + it.key()+ " updated!" + WHITE + "\n";
        }

        //Check updates for abilities
        else if(p->attributeExists(it.key()) == true) {
            p->setStat(it.key(), p->getStat(it.key())+val);
            sUpdated += GREEN + it.key() + " updated!" + WHITE + "\n";
        }
    }

    m_jUpdates.clear();
}
