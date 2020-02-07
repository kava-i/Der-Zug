#include "CDialogContext.hpp"
#include "CPlayer.hpp"

CDialogContext::CDialogContext(CPlayer* p)
{
    //Set permeability
    m_permeable = false;
    m_curPermeable = m_permeable;

    //Set first state
    setCurState("START", p);
}

//Modify handlers
void CDialogContext::setCurState(std::string newState, CPlayer* p)
{
    m_curState = newState;

    std::vector<size_t> activeOptions = p->getDialog()->states[newState]->getActiveOptions(p);
    m_eventmanager.clear();
    add_listener("help", &CContext::h_help);
    for(auto opt : activeOptions)
        add_listener(std::to_string(opt), &CContext::h_call);
}


// ***** HANDLERS ***** //

void CDialogContext::h_call(string& sIdentifier, CPlayer* p)
{
    string nextState = p->getDialog()->states[m_curState]->getNextState(sIdentifier, p);
    if(nextState == "")
        p->appendPrint("No valid option.\n");
    else
    {
        setCurState(nextState, p);
        std::string newCommand = p->getDialog()->states[m_curState]->callState(p);
        if(newCommand != "")
            p->throw_event(newCommand);
    }
}

void CDialogContext::error(CPlayer* p) {
    p->appendPrint("Dialog: Please choose a number, or enter \"help\".\n");
}
