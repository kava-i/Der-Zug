#include "CFightContext.hpp"
#include "CPlayer.hpp"


// ***** PARSER ***** //
vector<CContext::event> CFightContext::parser(string sInput, CPlayer* p)
{
    std::cout << "fightParser: " << sInput << std::endl;

    std::regex help("help");
    std::regex show("(show) (.*)");
    std::smatch m;
    if(std::regex_match(sInput, help))
        return {std::make_pair("help", "fight.txt")};
    else if(std::regex_search(sInput, m, show))
        return {std::make_pair("show", m[2])};
    
    string selectedAttack = p->getAttack(sInput);
    if(selectedAttack == "")
        return {std::make_pair("attackNotFound", "")};
    else
        return {std::make_pair("choose", sInput)};
}


// ***** HANDLERS ***** //
void CFightContext::h_choose(string& sIdentifier, CPlayer* p) {
    p->throw_event(p->getFight()->fightRound((sIdentifier))); 
}

void CFightContext::h_show(string& sIdentifier, CPlayer* p) {
    if(sIdentifier == "stats")
        p->appendPrint(p->getFight()->printStats(p));
    else if(sIdentifier == "opponent stats")
        p->appendPrint(p->getFight()->printStats(p->getFight()->getOpponent()));
}

void CFightContext::h_error(string&, CPlayer* p) {
    p->appendPrint("I can't do this. Please choose an attack, or use help.\n");
}
