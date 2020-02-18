#include "CChatContext.hpp"
#include "CPlayer.hpp"

//Alternate throwing events

void CChatContext::throw_event(event newEvent, CPlayer* p)
{
    std::cout << "Halllo!!\n";

    if(newEvent.first == "end")
        h_end(newEvent.second, p);
    else
        h_send(newEvent.second, p);
}

void CChatContext::h_send(string& sInput, CPlayer* p)
{
    std::cout << "... sending.\n";
    m_chatPartner->send(p->getName() + " \"" +sInput+"\"\n\n");
    p->appendPrint("YOU " + sInput + "\n");
}

void CChatContext::h_end(string& sMessage, CPlayer* p)
{
    if(sMessage == "")
        m_chatPartner->send("[Gespräch beendet].\n");
    else
        m_chatPartner->send(p->getName() + " \"" + sMessage + "\" [Gespräch beendet].\n");

    m_chatPartner->getContexts().erase("chat");

    p->appendPrint("Gespräch mit " + m_chatPartner->getName() + " beendet.\n");
    p->getContexts().erase("chat");
}



