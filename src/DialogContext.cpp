#include "CDialogContext.hpp"
#include "CPlayer.hpp"

CDialogContext::CDialogContext()
{
    //Set permeability
    m_permeable = false;

    //Set first state
    m_curState="START";

    //Add listeners
    add_listener("call", &CContext::h_call); 
    add_listener("error", &CContext::h_error);
    add_listener("choose", &CContext::h_call);
    add_listener("help", &CContext::h_help);
}


// ***** PARSER ***** //
vector<CContext::event> CDialogContext::parser(string sInput, CPlayer* p)
{
    std::cout << "dialogParser: " << sInput << std::endl;
    std::regex help("help");
    std::smatch m;
    if(std::regex_match(sInput, help))
        return {std::make_pair("help", "dialog.txt")};

    if(std::isdigit(sInput[0]) == false) 
        return {std::make_pair("error", "")};
    else
        return {std::make_pair("choose", sInput)};
}


// ***** HANDLERS ***** //

void CDialogContext::h_call(string& sIdentifier, CPlayer* p)
{
    string nextState = p->getDialog()->states[m_curState]->getNextState(sIdentifier, p);
    if(nextState == "")
        p->appendPrint("No valid option.\n");
    else{
        m_curState = nextState;
        p->throw_event(p->getDialog()->states[m_curState]->callState(p));
    }
}

void CDialogContext::h_error(string&, CPlayer* p) {
    p->appendPrint("Please choose a number, or enter \"help\".\n");
}
