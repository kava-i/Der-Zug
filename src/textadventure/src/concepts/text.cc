#include "text.h"
#include "objects/player.h"
#include "tools/logic_parser.h"
#include "tools/webcmd.h"

#define WHITE Webcmd::set_color(Webcmd::color::WHITE)
#define WHITEDARK Webcmd::set_color(Webcmd::color::WHITEDARK)
#define DARK Webcmd::set_color(Webcmd::color::DARK)
#define GREEN Webcmd::set_color(Webcmd::color::GREEN)

CText::CText(nlohmann::json jAttributes, CPlayer* p) {
  player_ = p;
  for(auto atts : jAttributes) 
    texts_.push_back(new COutput(atts));
}

std::string CText::print(bool events) {
  std::string sOutput = "";
  for(size_t i=0; i<texts_.size(); i++)
    sOutput += texts_[i]->print(player_, events);
  return sOutput;
}

std::string CText::reducedPrint(bool events) {
  std::string sOutput = "";
  for(size_t i=0; i<texts_.size(); i++)
    sOutput += texts_[i]->reducedPrint(player_, events);
  return sOutput;
}

std::string CText::pagePrint(size_t page) {
  std::string sOutput = "";
  for(size_t i=0; i<texts_.size(); i++) {
    if(texts_[i]->getPage() == page)
      sOutput += texts_[i]->reducedPrint(player_, true);
  }
  return sOutput;
}

size_t CText::getNumPages() {
  size_t page=0;
  size_t counter = 1;
  for(size_t i=0; i<texts_.size(); i++) {
    if(texts_[i]->getPage() > page)
      counter++;
  }
  return counter;
}

bool CText::underline(size_t page, std::string str1, std::string str2) {
  bool check = false;
  for(size_t i=0; i<texts_.size(); i++) {
    if(texts_[i]->getPage() == page) {
      bool c = texts_[i]->underline(str1, str2);
      if(c==true) check=true;
    }
  }
  return check;
}
    
// ***** ***** COUTPUT ***** ***** //
COutput::COutput(nlohmann::json jAttributes) 
  : updates_(jAttributes.value("updates", nlohmann::json::array())) 
{
  speaker_ = jAttributes.value("speaker", "");
  text_ = jAttributes.value("text", ""); 

  //Add dependencies and upgrades
  logic_ = jAttributes.value("logic", "");

  //Add events
  pre_permanent_events_ = jAttributes.value("pre_pEvents", std::vector<std::string>());
  pre_one_time_events_ = jAttributes.value("pre_otEvents", std::vector<std::string>());
  post_permanent_events_ = jAttributes.value("post_pEvents", std::vector<std::string>());
  post_one_time_events_ = jAttributes.value("post_otEvents", std::vector<std::string>());
  music_ = jAttributes.value("music", "");
  image_ = jAttributes.value("image", "");

  //In case of book, or read-item
  page_ = stoi(jAttributes.value("page", (std::string)"0")); 
}

std::string COutput::getSpeaker() {
  return speaker_;
}
std::string COutput::getText() {
  return text_;
}
size_t COutput::getPage() {
  return page_;
}

std::string COutput::print(CPlayer* p, bool events) {
  // Update speacker
  std::string speaker = speaker_;
  if(speaker_ != "" && speaker_ != "indent" && std::islower(speaker_[0]))
    speaker = p->getWorld()->getConfig()["printing"][speaker_];

  // Variables
  std::string mind_success = "";  //Storing, when success of mind will be announced 
  std::string updated = "";  //Storing, when abilities/ minds are updated.

  // Check dependencies
  if (checkDependencies(mind_success, p) == false)
    return ""; 
  
  // Update mind attributes
	p->Update(updates_, updated);
	updates_.Clear();

  // Add events to players staged events
  if(events == true)
    addEvents(p);  

  // Create output:
  std::string music = p->getContextMusic(music_);
  std::string image = p->getContextImage(image_);
  std::string output = ((music != "") ? music : "") + ((image != "") ? image : "") + text_ + "$";
	// If stats/ minds where updated: add newline before printing these.
	if (updated.length() > 0)
		output += "\n" + updated + "$";

  // Return text 
	if (speaker_ == "indent")
    return p->returnBlackPrint(output);

  return p->returnSpeakerPrint(speaker + mind_success, output);
}

std::string COutput::reducedPrint(CPlayer* p, bool events) {
  //Variables
  std::string mind_success = "";  //Storing, when success of mind will be announced 

  //Check dependencies
  if(checkDependencies(mind_success, p) == false)
    return ""; 

  //Update mind attributes
	p->Update(updates_);
	updates_.Clear();

  //Add events to players staged events
  if(events==true)
    addEvents(p);  

  //Return text
  return text_;
}

bool COutput::checkDependencies(std::string& mind_success, CPlayer* p) {

  // Check dependencies
  LogicParser logic(p->GetCurrentStatus());
  if(logic.Success(logic_) == false)
    return false;
  
  /*
	 *  TODO (fux): re-add this part and/ or enable it via config
	 *	//Create success output:
	 *	for(auto it=m_jDeps.begin(); it!=m_jDeps.end(); it++) {
	 *		if(p->getMinds().count(it.key()) > 0) {
	 *			sSuccess = DARK + " (level " + std::to_string(p->getMind(it.key()).level) 
	 *				+ ": Erfolg)" + WHITE;
	 *		}
	 *	}
	 */

  return true;
}

/**
* Add events to player staged events.
*/
void COutput::addEvents(CPlayer* p) {
  for(const auto &it : pre_permanent_events_)
    p->addPreEvent(it);
  for(const auto &it : pre_one_time_events_)
    p->addPreEvent(it);
  pre_one_time_events_.clear();
  for(const auto &it : post_permanent_events_)
    p->addPostEvent(it);
  for(const auto &it : post_one_time_events_)
    p->addPostEvent(it);
  post_one_time_events_.clear(); 
}

/**
* Try to underline text.
*/
bool COutput::underline(std::string str1, std::string str2) {
  std::string str = text_;
  func::convertToLower(str);
  size_t pos1 = str.find(str1);
  size_t pos2 = str.find(str2);
  if(pos1 == std::string::npos || pos2 == std::string::npos)
    return false;
  text_.insert(pos1-1, "<u>");
  text_.insert(pos2+3, "</u>");
  return true;
}
