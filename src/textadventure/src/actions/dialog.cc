#include "dialog.h"
#include "objects/player.h"
#include "tools/logic_parser.h"

// ***** ***** CDialog ***** *****

CDialog::CDialog(std::string sName, std::map<std::string, CDState*> states) {
  m_sName = sName;
  m_states = states;
}

std::string CDialog::getName() {
  return m_sName;
}

std::map<std::string, CDState*>& CDialog::getStates() {
  return m_states;
}

CDState* CDialog::getState(std::string sID) {
  if(m_states.count(sID) > 0)
      return m_states[sID];
  std::cout << "FATAL!!! Trying to access dialog-state that does not exits: " 
    << sID << std::endl;
  return nullptr;
}

void CDialog::setName(std::string sName) {
  m_sName = sName;
}

void CDialog::setStates(std::map<std::string, CDState*> states) {
  m_states = states;
}

// *** VARIOUS FUNCTIONS *** // 

std::string CDialog::visited() {
  std::string str = "";
  for (auto it : m_states) {
    if (it.second->visited() == true)
      str += it.first + ";";
  }
  return str.substr(0, str.length()-1);
}


void CDialog::addDialogOption(string sStateID, size_t optID) {
  m_states[sStateID]->getOptions()[m_states[sStateID]->numOptions()+1] = 
    m_states[sStateID]->getOptions()[-1];
}

void CDialog::deleteDialogOption(string sStateID, size_t optID) {
  m_states[sStateID]->getOptions().erase(optID);

  for(int i=optID+1; i<m_states[sStateID]->numOptions()+2; i++) {
    auto nodeHandler = m_states[sStateID]->getOptions().extract(i);
    nodeHandler.key() = i-1;
    m_states[sStateID]->getOptions().insert(std::move(nodeHandler));
  }
} 

void CDialog::moveStart(string sStateID) {
  m_states["START"] = m_states[sStateID];
}


void CDialog::changeDialog(string sCharacter, string sDialog, CPlayer* p) {
  auto lambda = [](CPerson* person) { return person->getName(); };
  std::string character = func::getObjectId(p->getRoom()->getCharacters(), sCharacter, lambda);
  if(character == "")
    character = sCharacter;
  p->getWorld()->getCharacter(character)->setDialog(p->getWorld()->getDialog(sDialog));
}


// ***** ***** CDState ***** ***** //

CDState::CDState(nlohmann::json jAtts, dialogoptions opts, CDialog* dia, 
    CPlayer* p) {
  m_text = new CText(jAtts.value("text", nlohmann::json::array()), p);
  m_sFunction = jAtts.value("function", "standard");
  m_sActions = jAtts.value("actions", "");
  m_sEvents = jAtts.value("events", "");
  m_options = opts;
  m_dialog = dia;
  visited_ = false;
}

// *** GETTER *** //

string CDState::getText() { 
  return m_text->print(); 
}

CDState::dialogoptions& CDState::getOptions() { 
  return m_options; 
}

bool CDState::visited() {
  return visited_;
}

// *** SETTER *** //

// *** FUNCTIONS *** // 

std::map<string, string (CDState::*)(CPlayer* p)> CDState::m_functions = {};
void CDState::initializeFunctions() {
  m_functions["standard"]     = &CDState::standard;
  m_functions["toeten"]       = &CDState::toeten;
  m_functions["keinTicket"]   = &CDState::keinTicket;
  m_functions["strangeGuy1"]  = &CDState::strangeGuy1;
  m_functions["strangeGuy2"]  = &CDState::strangeGuy2;
}

string CDState::callState(CPlayer* p) {
  visited_ = true;
  return (this->*m_functions[m_sFunction])(p) + m_sEvents;
}

string CDState::getNextState(string sPlayerChoice, CPlayer* p) {
  p->set_subsitues({{"visited", m_dialog->visited()}});
  LogicParser logic(p->GetCurrentStatus());
  if(numOptions() < stoi(sPlayerChoice))
    return "";
  else if(logic.Success(m_options[stoi(sPlayerChoice)].logic_) == false)
    return "";
  else {
    m_options[stoi(sPlayerChoice)].visited = true;
    return m_options[stoi(sPlayerChoice)].sTarget;
  }
}

int CDState::numOptions() {
  int counter = 0;
  for(auto it : m_options) {
    if(it.first > 0)
      counter++;
  }
  return counter;
}

void CDState::executeActions(CPlayer* p) {
  std::vector<std::string> actions = func::split(m_sActions, ",");
  for(const auto& action : actions) {
    std::vector<std::string> parameters = func::split(action, "|");
    if(parameters[0] == "addDialogOption") 
      m_dialog->addDialogOption(parameters[1], std::stoi(parameters[2]));
    else if(parameters[0] == "deleteDialogOption")
      m_dialog->deleteDialogOption(parameters[1], std::stoi(parameters[2]));
    else if(parameters[0] == "changeDialog")
      m_dialog->changeDialog(parameters[1], parameters[2], p);
    else if(parameters[0] == "moveStart")
      m_dialog->moveStart(parameters[1]);
  }
}

// *** FUNCTION POINTER *** //
string CDState::standard(CPlayer* p) {
  p->set_subsitues({{"visited", m_dialog->visited()}});
  string sOutput = m_text->print();

  std::vector<size_t> activeOptions = getActiveOptions(p);
  size_t counter = 1;
  for(auto opt : activeOptions) {
    if(m_options[opt].visited==false)
      sOutput += std::to_string(counter) + ": " + m_options[opt].sText + "\n";
    else
      sOutput += "(" + std::to_string(counter) + ": " + m_options[opt].sText + ")\n";
    counter++;
  }
  
  //Execute actions after this dialog state (f.e. delete options, change text etc.)
  executeActions(p);
      
  if(activeOptions.size() == 0){
    p->appendPrint(sOutput + "Dialog ended.\n");
    return "endDialog";
  }

  p->appendPrint(sOutput);
  return "";
}

std::vector<size_t> CDState::getActiveOptions(CPlayer* p) {
  std::vector<size_t> activeOptions;
  size_t numOpts = numOptions();
  p->set_subsitues({{"visited", m_dialog->visited()}});
  LogicParser logic(p->GetCurrentStatus());
  for(size_t i=1; i<numOpts+1; i++) {
    if(logic.Success(m_options[i].logic_) == true)
      activeOptions.push_back(i);
  }
  return activeOptions;
}

// ***** SPECIAL STATE FUNCTIONS ***** //

string CDState::toeten(CPlayer* p) {
  string sOutput=standard(p);
  m_sEvents += ";killCharacter " + p->getCurDialogPartner()->getID();
  return sOutput; 
}

string CDState::keinTicket(CPlayer* p) {
  string sOutput=standard(p);
  if(p->getStat("gold") < 10)
    p->setNewQuest("geld_fuer_ticket");
  return sOutput;
}

string CDState::strangeGuy1(CPlayer* p) {
  string sOutput = standard(p);
  if(p->getWorld()->getQuest("komische_gruppe")->getActive() == false)
    p->setNewQuest("komische_gruppe");
  return sOutput;
}

string CDState::strangeGuy2(CPlayer* p) {
  string sOutput = standard(p);
  p->questSolved("komische_gruppe", "hilfe");
  return sOutput;
}
