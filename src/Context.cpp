#include "CContext.hpp"
#include "CPlayer.hpp"

// ***** GETTER ***** // 
bool CContext::getPermeable() { 
    return m_curPermeable; 
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

void CContext::throw_event(event e, CPlayer* p)
{
    m_curPermeable = m_permeable;
    m_block = false;

    if(m_eventmanager.count(e.first) == 0) {
        error(p);
        return;
    }
        
    for(auto it : m_eventmanager[e.first]) {
        if(m_block == true)
            break;
        (this->*it)(e.second, p);
    }
}

// ***** EVENTHANDLER ***** //
void CContext::h_help(string& sIdentifier, CPlayer* p) {
    std::ifstream read("factory/help/"+m_sHelp);

    string str((std::istreambuf_iterator<char>(read)),
                 std::istreambuf_iterator<char>());
    p->appendPrint("\nHELP: \n" + str);
}

