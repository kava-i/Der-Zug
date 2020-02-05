#include "CContext.hpp"
#include "CPlayer.hpp"

// ***** GETTER ***** // 
bool CContext::getPermeable() { return m_permeable; }


// **** Use Parser acctually belonging to CStandardContext.hpp as standardparser ***** //
vector<CContext::event> CContext::parser(string sInput, CPlayer*)
{
    std::cout << "standardParser: " << sInput << std::endl;
    //Create regular expressions for different command the player might have choosen
    std::regex show("(show) (.*)");
    std::regex examine("examine");
    std::regex lookIn("(look in )(.*)");
    std::regex pickUp("(pick up )(.*)");
    std::regex consume("(drink|eat|smoke) (.*)");
    std::regex equipe("(equipe) (.*)");
    std::regex dequipe("(dequipe) (.*)");
    std::regex goTo("(go to) (.*)");
    std::regex talkTo("(talk to) (.*)");
    std::regex help("help");
    std::regex end_direct(":q");
    std::regex tryMe("(try) (.*)");
    //Create an instans of smatch
    std::smatch m;

    //Show 
    if(std::regex_search(sInput, m, show))
        return {std::make_pair("show", m[2])};
    //Examine
    else if(std::regex_match(sInput, examine))
        return {std::make_pair("examine", "")};
    //Look in
    else if(std::regex_match(sInput, m, lookIn))
        return {std::make_pair("lookIn", m[2])};
    //Take
    else if(std::regex_match(sInput, m, pickUp))
        return {std::make_pair("take", m[2])}; 
    //Consume
    else if(std::regex_match(sInput, m, consume))
        return {std::make_pair("consume", m[2])};
    //Equipe
    else if(std::regex_match(sInput, m, equipe))
        return {std::make_pair("equipe", m[2])};
    //Dequipe
    else if(std::regex_match(sInput, m, dequipe))
        return {std::make_pair("dequipe", m[2])};
    //Change room
    else if(std::regex_match(sInput, m, goTo))
        return {std::make_pair("goTo", m[2])};
    //Talk to 
    else if(std::regex_match(sInput, m, talkTo))
        return {std::make_pair("talkTo", m[2])};
    //Help 
    else if(std::regex_match(sInput, help))
        return {std::make_pair("help", "standard.txt")};
    //Developer option
    else if(std::regex_match(sInput, m, tryMe))
        return {std::make_pair("try", m[2])}; 
    //Tutorial
    else if(sInput == "startTutorial")
        return {std::make_pair("startTutorial", "")};
    else
        return {std::make_pair("error", "")};
}

// **** FUNCTIONS ***** //
void CContext::add_listener(string sEventType, void(CContext::*handler)(string&, CPlayer*)) {
    m_eventmanager[sEventType].push_back(handler);
}

void CContext::add_listener(string sEventType, void(CContext::*handler)(string&, CPlayer*), size_t pos){
    m_eventmanager[sEventType].insert(m_eventmanager[sEventType].begin()+pos, handler);
}

void CContext::delete_listener(string sEventType, int index) {
    m_eventmanager[sEventType].erase(m_eventmanager[sEventType].begin()+index);
}

void CContext::throw_event(std::vector<event> events, CPlayer* p)
{
    bool thrown = false;
    for(auto e : events)
    {
        std::cout << e.first << ", " << e.second << "\n";

        if(m_eventmanager.count(e.first) == 0) 
            continue;
            
        thrown = true;
        for(auto it : m_eventmanager[e.first])
            (this->*it)(e.second, p);
    }
    
    if(thrown == false && m_permeable == false)
    {
        std::cout << "Nothing thrown." << std::endl;
        error(p);
    }
}

// ***** EVENTHANDLER ***** //
void CContext::h_help(string& sIdentifier, CPlayer* p) {
    std::ifstream read("factory/help/"+sIdentifier);

    string str((std::istreambuf_iterator<char>(read)),
                 std::istreambuf_iterator<char>());
    p->appendPrint(str);
}

