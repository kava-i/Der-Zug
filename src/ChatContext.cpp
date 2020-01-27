#include "CChatContext.hpp"
#include "CPlayer.hpp"

vector<CChatContext::event> CChatContext::parser(string sInput, CPlayer*)
{
    std::cout << "Chat parser: " << sInput << std::endl;

    std::regex help("help");
    std::regex endChat1("end");
    std::regex endChat2("(end) (.*)");
    std::smatch m;

    if(std::regex_match(sInput, help))
        return {std::make_pair("help", "chat.txt")};
    else if(std::regex_match(sInput, m, endChat1))
        return {std::make_pair("endChat", "")};
    else if(std::regex_match(sInput, m, endChat2))
        return {std::make_pair("endChat", m[2])};
    else
        return {std::make_pair("send", sInput)};
}


void CChatContext::h_send(string& sInput, CPlayer* p)
{
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



