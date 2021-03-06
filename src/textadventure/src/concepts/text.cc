#include "text.h"
#include "objects/player.h"
#include "tools/logic_parser.h"

#define WHITE Webcmd::set_color(Webcmd::color::WHITE)
#define WHITEDARK Webcmd::set_color(Webcmd::color::WHITEDARK)
#define DARK Webcmd::set_color(Webcmd::color::DARK)
#define GREEN Webcmd::set_color(Webcmd::color::GREEN)

CText::CText(nlohmann::json jAttributes, CPlayer* p) {
  m_player = p;
  for(auto atts : jAttributes) 
    m_texts.push_back(new COutput(atts));
}

std::string CText::print(bool events) {
    std::string sOutput = "";
    for(size_t i=0; i<m_texts.size(); i++)
      sOutput += m_texts[i]->print(m_player, events);
    return sOutput;
}

std::string CText::reducedPrint(bool events) {
  std::string sOutput = "";
  for(size_t i=0; i<m_texts.size(); i++)
    sOutput += m_texts[i]->reducedPrint(m_player, events);
  return sOutput;
}

std::string CText::pagePrint(size_t page) {
  std::string sOutput = "";
  for(size_t i=0; i<m_texts.size(); i++) {
    if(m_texts[i]->getPage() == page)
      sOutput += m_texts[i]->reducedPrint(m_player, true);
  }
  return sOutput;
}

size_t CText::getNumPages() {
  size_t page=0;
  size_t counter = 1;
  for(size_t i=0; i<m_texts.size(); i++) {
    if(m_texts[i]->getPage() > page)
      counter++;
  }
  return counter;
}

bool CText::underline(size_t page, std::string str1, std::string str2) {
  bool check = false;
  for(size_t i=0; i<m_texts.size(); i++) {
    if(m_texts[i]->getPage() == page) {
      bool c = m_texts[i]->underline(str1, str2);
      if(c==true) check=true;
    }
  }
  return check;
}
    
// ***** ***** COUTPUT ***** ***** //
COutput::COutput(nlohmann::json jAttributes) {
  m_sSpeaker = jAttributes.value("speaker", "");
  m_sText = jAttributes.value("text", ""); 

  //Add dependencies and upgrades
  logic_ = jAttributes.value("logic", "");
  m_jUpdates = jAttributes.value("updates", nlohmann::json::object());

  //Add events
  m_pre_permanentEvents = jAttributes.value("pre_pEvents", std::vector<std::string>());
  m_pre_oneTimeEvents = jAttributes.value("pre_otEvents", std::vector<std::string>());
  m_post_permanentEvents = jAttributes.value("post_pEvents", std::vector<std::string>());
  m_post_oneTimeEvents = jAttributes.value("post_otEvents", std::vector<std::string>());

  //In case of book, or read-item
  m_page = stoi(jAttributes.value("page", (std::string)"0")); 
}

std::string COutput::getSpeaker() {
  return m_sSpeaker;
}
std::string COutput::getText() {
  return m_sText;
}
size_t COutput::getPage() {
  return m_page;
}

std::string COutput::print(CPlayer* p, bool events) {
  //Update speacker
  std::string sSpeaker = m_sSpeaker;
  if(m_sSpeaker != "" && m_sSpeaker != "indent" && std::islower(m_sSpeaker[0]))
    sSpeaker = p->getWorld()->getConfig()["printing"][m_sSpeaker];

  //Variables
  std::string sSuccess = "";  //Storing, when success of mind will be announced 
  std::string sUpdated = "";  //Storing, when abilities/ minds are updated.

  //Check dependencies
  if(checkDependencies(sSuccess, p) == false)
    return ""; 
  
  //Update mind attributes
  updateAttrbutes(p, sUpdated);

  //Add events to players staged events
  if(events == true)
    addEvents(p);  

  //Return text 
  if(m_sSpeaker == "indent")
    return p->returnBlackPrint(m_sText + "$" + sUpdated);

  return p->returnSpeakerPrint(sSpeaker + sSuccess, m_sText + "$" + sUpdated);
}

std::string COutput::reducedPrint(CPlayer* p, bool events) {
  //Variables
  std::string sSuccess = "";  //Storing, when success of mind will be announced 

  //Check dependencies
  if(checkDependencies(sSuccess, p) == false)
    return ""; 

  //Update mind attributes
  updateAttrbutes(p);

  //Add events to players staged events
  if(events==true)
    addEvents(p);  

  //Return text
  return m_sText;
}

bool COutput::checkDependencies(std::string& sSuccess, CPlayer* p) {

  //Check dependencies
  LogicParser logic(p->GetCurrentStatus());
  if(logic.Success(logic_) == false)
    return false;
  
  /*
  //Create success output:
  for(auto it=m_jDeps.begin(); it!=m_jDeps.end(); it++) {
    if(p->getMinds().count(it.key()) > 0) {
      sSuccess = DARK + " (level " + std::to_string(p->getMind(it.key()).level) 
        + ": Erfolg)" + WHITE;
    }
  }*/

  return true;
}

void COutput::updateAttrbutes(CPlayer* p, std::string& sUpdated) {
  for(auto it=m_jUpdates.begin(); it!= m_jUpdates.end(); it++) {
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

void COutput::updateAttrbutes(CPlayer* p) {
  for(auto it=m_jUpdates.begin(); it!= m_jUpdates.end(); it++) {
    int val=it.value();

    //Check updates for minds
    if(p->getMinds().count(it.key()) > 0) 
      p->getMind(it.key()).level += val;

    //Check updates for abilities
    else if(p->attributeExists(it.key()) == true)
      p->setStat(it.key(), p->getStat(it.key())+val);
  }
  m_jUpdates.clear();
}

/**
* Add events to player staged events.
*/
void COutput::addEvents(CPlayer* p) {
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

/**
* Try to underline text.
*/
bool COutput::underline(std::string str1, std::string str2) {
  std::string str = m_sText;
  func::convertToLower(str);
  size_t pos1 = str.find(str1);
  size_t pos2 = str.find(str2);
  if(pos1 == std::string::npos || pos2 == std::string::npos)
    return false;
  m_sText.insert(pos1-1, "<u>");
  m_sText.insert(pos2+3, "</u>");
  return true;
}
