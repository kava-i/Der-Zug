#include "CDialog.hpp"
#include "CPlayer.hpp"

// ***** ***** CDialog ***** *****
CDialog::CDialog(std::string sName, std::map<std::string, CDState*> states)
{
    m_sName = sName;
    m_states = states;
}

///Getter for name of dialog.
std::string CDialog::getName() {
    return m_sName;
}

///Getter for map of states in dialog.
std::map<std::string, CDState*>& CDialog::getStates() {
    return m_states;
}

///Getter for a single state
CDState* CDialog::getState(std::string sID) {
    if(m_states.count(sID) > 0)
        return m_states[sID];
    std::cout << "FATAL!!! Trying to access dialog-state that does not exits: " << sID << std::endl;
    return nullptr;
}


///Setter for dialog name.
void CDialog::setName(std::string sName) {
    m_sName = sName;
}

///Setter for dialog states.
void CDialog::setStates(std::map<std::string, CDState*> states) {
    m_states = states;
}

// *** VARIOUS FUNCTIONS *** // 
void CDialog::changeStateText(string sStateID, size_t text) {
    m_states[sStateID]->setText(text);
}
void CDialog::addDialogOption(string sStateID, size_t optID) {
    m_states[sStateID]->getOptions()[m_states[sStateID]->numOptions()+1] = m_states[sStateID]->getOptions()[-1];
}

void CDialog::deleteDialogOption(string sStateID, size_t optID) {
    m_states[sStateID]->getOptions().erase(optID);

    for(int i=optID+1; i<m_states[sStateID]->numOptions()+2; i++)
    {
        auto nodeHandler = m_states[sStateID]->getOptions().extract(i);
        nodeHandler.key() = i-1;
        m_states[sStateID]->getOptions().insert(std::move(nodeHandler));
    }
} 

void CDialog::changeDialog(string sCharacter, string sDialog, CPlayer* p)
{
    p->getWorld()->getCharacter(sCharacter)->setDialog(p->getWorld()->getDialog(sDialog));
}

// ***** ***** CDState ***** ***** //
CDState::CDState(nlohmann::json jAtts, dialogoptions opts, CDialog* dia, CPlayer* p)
{
    m_text = new CText(jAtts.value("text", nlohmann::json::array()), p);
    m_sFunction = jAtts.value("function", "standard");
    m_sActions = jAtts.value("actions", "");
    m_sEvents = jAtts.value("events", "");

    //Parse alternative texts
    std::vector<CText*> altTexts;
    if(jAtts.count("altTexts") > 0) {
        for(auto text : jAtts["altTexts"])
            altTexts.push_back(new CText(text, p));
    }
    m_alternativeTexts = altTexts;
    m_options = opts;
    m_dialog = dia;
}

// *** GETTER *** //

///Getter for returning text of dialog-state.
string CDState::getText() { 
    return m_text->print(); 
}

///Getter for returning options of dialog-state.
CDState::dialogoptions& CDState::getOptions() { 
    return m_options; 
}

// *** SETTER *** //

///Setter for text of dialog-state.
void CDState::setText(size_t text) { 
    m_text = m_alternativeTexts[text]; 
}

// *** FUNCTIONS *** // 

std::map<string, string (CDState::*)(CPlayer* p)> CDState::m_functions = {};
void CDState::initializeFunctions()
{
    m_functions["standard"]     = &CDState::standard;
    m_functions["keinTicket"]   = &CDState::keinTicket;
    m_functions["strangeGuy1"]   = &CDState::strangeGuy1;
    m_functions["strangeGuy2"]   = &CDState::strangeGuy2;
}


string CDState::callState(CPlayer* p) {
    std::string sReturn = (this->*m_functions[m_sFunction])(p) + m_sEvents;
    std::cout << "Events: " << sReturn << std::endl;
    return sReturn;
}

string CDState::getNextState(string sPlayerChoice, CPlayer* p)
{
    if(numOptions() < stoi(sPlayerChoice))
        return "";
    else if(p->checkDependencies(m_options[stoi(sPlayerChoice)].jDependencys) == false)
        return "";
    else
        return m_options[stoi(sPlayerChoice)].sTarget;
}

int CDState::numOptions()
{
    int counter = 0;
    for(auto it : m_options) {
        if(it.first > 0)
            counter++;
    }
    return counter;
}

void CDState::executeActions(CPlayer* p)
{
    std::vector<std::string> actions = func::split(m_sActions, "|");
    for(const auto& action : actions)
    {
        std::vector<std::string> parameters = func::split(action, ",");
        if(parameters[0] == "addDialogOption") 
            m_dialog->addDialogOption(parameters[1], std::stoi(parameters[2]));
        else if(parameters[0] == "deleteDialogOption")
            m_dialog->deleteDialogOption(parameters[1], std::stoi(parameters[2]));
        else if(parameters[0] == "changeStateText")
            m_dialog->changeStateText(parameters[1], std::stoi(parameters[2]));
        else if(parameters[0] == "changeDialog")
            m_dialog->changeDialog(parameters[1], parameters[2], p);
        else if(parameters[0] == "setNewQuest")
            p->setNewQuest(parameters[1]);
    }
}

// *** FUNCTION POINTER *** //
string CDState::standard(CPlayer* p)
{
    string sOutput = m_text->print();

    std::vector<size_t> activeOptions = getActiveOptions(p);
    size_t counter = 1;
    for(auto opt : activeOptions)
    {
        sOutput += std::to_string(counter) + ": " + m_options[opt].sText + "\n";
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

std::vector<size_t> CDState::getActiveOptions(CPlayer* p)
{
    std::vector<size_t> activeOptions;
    size_t numOpts = numOptions();
    for(size_t i=1; i<numOpts+1; i++) {
        if(p->checkDependencies(m_options[i].jDependencys) == true)
            activeOptions.push_back(i);
    }
    return activeOptions;
}



// ***** SPECIAL STATE FUNCTIONS ***** //

string CDState::keinTicket(CPlayer* p)
{
    string sOutput=standard(p);
    if(p->getStat("gold") < 10)
        p->setNewQuest("geld_fuer_ticket");
    return sOutput;
}

string CDState::strangeGuy1(CPlayer* p)
{
    string sOutput = standard(p);
    if(p->getWorld()->getQuest("komische_gruppe")->getActive() == false)
        p->setNewQuest("komische_gruppe");
    return sOutput;
}

string CDState::strangeGuy2(CPlayer* p)
{
    string sOutput = standard(p);
    p->throw_events("recieveMoney 3");
    p->questSolved("komische_gruppe", "2hilfe");

    m_dialog->deleteDialogOption("START", 1);

    return sOutput;
}


