#include "CChoiceContext.hpp"
#include "CPlayer.hpp"

CChoiceContext::CChoiceContext(string obj, std::string sError)
{
    //Set permeability
    m_permeable = false;
    m_curPermeable = m_permeable;

    //Set object
    m_sObject=obj;
    m_sError = sError;
}

CChoiceContext::CChoiceContext(std::string obj, objectmap& mapObjects)
{
    m_permeable=true;
    m_curPermeable=m_permeable;
    m_sObject = obj;
    m_objectMap = mapObjects;
} 
    

// ***** Functions ***** //
void CChoiceContext::error(CPlayer* p)
{
    p->appendPrint(m_sError);
}


// ***** Handlers ***** //

void CChoiceContext::h_select(string& sIdentifier, CPlayer* p)
{
    std::cout << "h_select: " << sIdentifier << ", " << m_sObject << ", " << std::endl;
    std::string obj = p->getObject(m_objectMap, sIdentifier);
    if(obj == "")
        p->appendPrint("Choose an number or a new Command\n");

    else
    {
        std::cout << "Found Object: " << obj << std::endl;
        p->throw_event(m_sObject + " " + obj);
        m_permeable=false;
        p->getContexts().erase("choice"); 
    }
}

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
        p->appendPrint("You chose not to equipe " + p->getItem_byID(m_sObject)->getName() + ".\n");
        p->getContexts().erase("choice");
    }

    else
        p->appendPrint("Worng input. Use \"help\" if you're unsure what to do.\n");
}

void CChoiceContext::h_updateMind(string& sIdentifier, CPlayer* p)
{
    std::string mind="";
    for(auto it : p->getMinds())
    {
        if(fuzzy::fuzzy_cmp(func::returnToLower(it.first), sIdentifier) <= 0.2)
            mind = it.first;
    }

    if(mind == "")
        p->appendPrint("Falsche Eingabe!\n");

    else
    {
        p->getMinds()[mind]++;
        p->appendPrint(sIdentifier + " updated by 1\n");
        int num = stoi(m_sObject)-1;
        std::cout << "Object: " << m_sObject << "\nNum: " << num << std::endl;
        p->getContexts().erase("choice");
        if(num>0)
            p->updateMinds(num);
    }
}
