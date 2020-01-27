#include "CContext.hpp"
#include "CPlayer.hpp"

// ***** GETTER ***** // 
bool CContext::getPermeable() { return m_permeable; }


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

void CContext::throw_event(std::string sInput, CPlayer* p)
{
    if(sInput == "")
        return; 

    vector<event> events = parser(sInput, p);

    for(auto e : events)
    {
        std::cout << e.first << ", " << e.second << "\n";

        if(m_eventmanager.count(e.first) == 0) 
            return;
            
        for(auto it : m_eventmanager[e.first])
            (this->*it)(e.second, p);
    }
}



// ***** EVENTHANDLER ***** //
void CContext::h_help(string& sIdentifier, CPlayer* p) {
    std::ifstream read("factory/help/"+sIdentifier);

    string str((std::istreambuf_iterator<char>(read)),
                 std::istreambuf_iterator<char>());
    p->appendPrint(str);
}

