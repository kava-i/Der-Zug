#include "CChatContext.hpp"
#include "CPlayer.hpp"

//Alternate throwing events

bool CChatContext::throw_event(event newEvent, CPlayer* p)
{
    if(newEvent.first.find("[end]") != std::string::npos)
        h_end(newEvent.second, p);
    else if(newEvent.first == newEvent.second)
        h_send(newEvent.first, p);
    else
    {
        std::string msg = newEvent.first + " " + newEvent.second;
        h_send(msg, p);
    }

    return m_permeable;
}

void CChatContext::h_send(string& sInput, CPlayer* p)
{
    std::cout << "... sending.\n";
    m_chatPartner->send(func::returnToUpper(p->getName()) + " \"" +sInput+"\"\n\n");
    p->appendPrint("YOU " + sInput + "\n");
}

void CChatContext::h_end(string& sMessage, CPlayer* p)
{
    if(sMessage == "[end]")
        m_chatPartner->send("[Gespräch beendet].\n");
    else
        m_chatPartner->send(func::returnToUpper(p->getName())+" \""+sMessage+"\" [Gespräch beendet].\n");

    m_chatPartner->getContexts().erase("chat");

    p->appendPrint("Gespräch mit " + m_chatPartner->getName() + " beendet.\n");
    p->getContexts().erase("chat");
}



