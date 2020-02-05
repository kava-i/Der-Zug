#include "CContext.hpp"
#include "CPlayer.hpp"

// ***** GETTER ***** // 
bool CContext::getPermeable() { return m_permeable; }


void CContext::setGame(CGame *)
{
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

