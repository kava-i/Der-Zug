#include "CDialog.hpp"
#include "CPlayer.hpp"

CDState::CDState(string sText, string func, vector<string> alternativeTexts, dialogoptions opts, SDialog* dia)
{
    m_sText = sText;
    m_sFunction = func;
    m_alternativeTexts = alternativeTexts;
    m_options = opts;
    m_dialog = dia;
}

// *** GETTER *** //
string CDState::getText() { return m_sText; }
CDState::dialogoptions& CDState::getOptions() { return m_options; }

// *** SETTER *** //
void CDState::setText(size_t text) { m_sText = m_alternativeTexts[text]; }

// *** FUNCTIONS *** // 

std::map<string, string (CDState::*)(CPlayer* p)> CDState::m_functions = {};
void CDState::initializeFunctions()
{
    m_functions["standard"]     = &CDState::standard;
    m_functions["parsen1"]      = &CDState::parsen1;
    m_functions["parsen2"]      = &CDState::parsen2;
    m_functions["pissingman1"]  = &CDState::pissingman1;
    m_functions["ticket"]       = &CDState::ticket;
    m_functions["betrunkene"]   = &CDState::betrunkene;
}


string CDState::callState(CPlayer* p) {
    return (this->*m_functions[m_sFunction])(p);
}

string CDState::getNextState(string sPlayerChoice, CPlayer* p)
{
    if(numOptions() < stoi(sPlayerChoice))
        return "";
    else if(checkDependencys(m_options[stoi(sPlayerChoice)], p) == false)
        return "";
    else
        return m_options[stoi(sPlayerChoice)].sTarget;
}

// *** FUNCTION POINTER *** //
string CDState::standard(CPlayer* p)
{
    string sOutput = m_sText + "\n";

    if(numOptions() == 0) {
        p->appendPrint(sOutput + "Dialog ended. \n");
        return "endDialog";
    }

    size_t numOpts = numOptions();
    bool options = false;
    for(size_t i=1; i<numOpts+1; i++)
    {
        if(checkDependencys(m_options[i], p) == true)
        {
            options = true;
            sOutput += std::to_string(i) + ": " + m_options[i].sText + "\n";
        }
    }

    if(options == false){
        p->appendPrint(sOutput + "Dialog ended.\n");
        return "endDialog";
     }

    p->appendPrint(sOutput);
    return "";
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
    sOutput+="/fight_parsen";
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
    string sOutput = standard(p); 
    return sOutput+"/addItem_ticket";
}

string CDState::betrunkene(CPlayer* p)
{
    string sOutput = standard(p);
    p->appendPrint("$");
    sOutput+="/fight_besoffene_frau";
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
    p->getWorld()->getCharacters()[sCharacter]->setDialog(p->getWorld()->dialogFactory(sDialog));
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

bool CDState::checkDependencys(SDOption& option, CPlayer* p)
{
    if(option.jDependencys.size() == 0)
        return true;
    for(auto it=option.jDependencys.begin(); it!=option.jDependencys.end(); ++it)
    {
        if(it.key() == "highness" && p->getHighness() < it.value())
            return false;
        if(it.key() == "gold")
        {
            int gold = it.value();
            std::cout << "GOLD: " << gold << std::endl;
            if(gold < 0 && gold*(-1) < p->getGold())
            {
                std::cout << "GOLD: " << gold*(-1) << " - "  << p->getGold() << std::endl;
                return false;
            }
            else if(gold > 0 && gold > p->getGold())
                return false;
       }
    }

    return true;
}
