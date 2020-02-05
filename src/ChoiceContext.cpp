#include "CChoiceContext.hpp"
#include "CPlayer.hpp"

CChoiceContext::CChoiceContext(string obj)
{
    //Set permeability
    m_permeable = false;

    //Set object
    m_sObject=obj;
}

// ***** Handlers ***** //

void CChoiceContext::h_choose_equipe(string& sIdentifier, CPlayer* p)
{
    std::cout << "h_choose_equipe: " << sIdentifier << " " << m_sObject << std::endl;
    if(sIdentifier == "yes")
    {
        p->dequipeItem("weapon");
        p->equipeItem(p->getItem_byID(m_sObject), "weapon");
        p->getContexts().erase("choice");
    }

    else if(sIdentifier == "no")
    {
        std::cout << "1." << std::endl;
        p->appendPrint("You chose not to equipe " + p->getItem_byID(m_sObject)->getName() + ".\n");
        std::cout << "2." << std::endl;
        p->getContexts().erase("choice");
        std::cout << "3." << std::endl;
    }

    else
        p->appendPrint("Worng input. Use \"help\" if you're unsure what to do.\n");
}
