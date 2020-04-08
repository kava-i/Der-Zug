#include "CDialog.hpp"
#include "CPlayer.hpp"

CDState::CDState(nlohmann::json jAtts, dialogoptions opts, SDialog* dia, CPlayer* p)
{
    m_text = new CText(jAtts.value("text", nlohmann::json::array()), p);
    m_sFunction = jAtts.value("function", "standard");

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
string CDState::getText() { return m_text->print(); }
CDState::dialogoptions& CDState::getOptions() { return m_options; }

// *** SETTER *** //
void CDState::setText(size_t text) { m_text = m_alternativeTexts[text]; }

// *** FUNCTIONS *** // 

std::map<string, string (CDState::*)(CPlayer* p)> CDState::m_functions = {};
void CDState::initializeFunctions()
{
    m_functions["standard"]     = &CDState::standard;
    m_functions["parsen1"]      = &CDState::parsen1;
    m_functions["parsen2"]      = &CDState::parsen2;
    m_functions["pissingman1"]  = &CDState::pissingman1;
    m_functions["ticket"]       = &CDState::ticket;
    m_functions["keinTicket"]   = &CDState::keinTicket;
    m_functions["betrunkene"]   = &CDState::betrunkene;
    m_functions["strangeGuy1"]   = &CDState::strangeGuy1;
    m_functions["strangeGuy2"]   = &CDState::strangeGuy2;
}


string CDState::callState(CPlayer* p) {
    return (this->*m_functions[m_sFunction])(p);
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

// *** FUNCTION POINTER *** //
string CDState::standard(CPlayer* p)
{
    string sOutput = m_text->print() + "\n";

    if(numOptions() == 0) {
        p->appendPrint(sOutput + "Dialog ended. \n");
        return "endDialog";
    }

    std::vector<size_t> activeOptions = getActiveOptions(p);
    size_t counter = 1;
    for(auto opt : activeOptions)
    {
        sOutput += std::to_string(counter) + ": " + m_options[opt].sText + "\n";
        counter++;
    }
        
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

string CDState::parsen1(CPlayer* p)
{
    string sOutput = standard(p);
    addDialogOption("START", -1);       //Add new option (4)
    deleteDialogOption("START", 1);     //Delete old option (1)
    changeStateText("START", 0);        //Change text (0)
    m_sFunction = "standard";           
    return sOutput;
}

string CDState::parsen2(CPlayer* p)
{
    string sOutput = standard(p);
    p->appendPrint("$");
    sOutput+=";fight parsen";
    return sOutput;
}

string CDState::pissingman1(CPlayer* p)
{
    string sOutput = standard(p);
    changeDialog("pissing_man", "defaultDialog", p);
    return sOutput;
} 

string CDState::ticket(CPlayer* p)
{
    deleteDialogOption("START", 1);
    string sOutput = standard(p); 
    return sOutput+";addItem ticket";
}

string CDState::keinTicket(CPlayer* p)
{
    string sOutput=standard(p);
    if(p->getStat("gold") < 10)
        p->setNewQuest("geld_fuer_ticket");
    return sOutput;
}

string CDState::betrunkene(CPlayer* p)
{
    string sOutput = standard(p);
    p->appendPrint("$");
    sOutput+=";fight besoffene_frau";
    p->setNewQuest("besoffene_frau");
    return sOutput;
}

string CDState::strangeGuy1(CPlayer* p)
{
    string sOutput = standard(p);
    if(p->getWorld()->getQuests()["komische_gruppe"]->getActive() == false)
        p->setNewQuest("komische_gruppe");
    return sOutput;
}

string CDState::strangeGuy2(CPlayer* p)
{
    string sOutput = standard(p);
    p->throw_event("recieveMoney 3");
    p->questSolved("komische_gruppe", "2hilfe");
    deleteDialogOption("die_gruppe", 1);  
    return sOutput;
}

// *** VARIOUS FUNCTIONS *** // 
void CDState::changeStateText(string sStateID, size_t text) {
    m_dialog->states["START"]->setText(text);
}
void CDState::addDialogOption(string sStateID, size_t optID) {
    m_dialog->states["START"]->getOptions()[m_dialog->states["START"]->numOptions()+1] = m_dialog->states["START"]->getOptions()[-1];
}

void CDState::deleteDialogOption(string sStateID, size_t optID) {
    m_dialog->states["START"]->getOptions().erase(optID);

    for(int i=optID+1; i<m_dialog->states["START"]->numOptions()+2; i++)
    {
        auto nodeHandler = m_dialog->states["START"]->getOptions().extract(i);
        nodeHandler.key() = i-1;
        m_dialog->states["START"]->getOptions().insert(std::move(nodeHandler));
    }
} 

void CDState::changeDialog(string sCharacter, string sDialog, CPlayer* p)
{
    p->getWorld()->getCharacters()[sCharacter]->setDialog(p->getWorld()->dialogFactory(sDialog, p));
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
