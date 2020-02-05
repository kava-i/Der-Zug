#include "CQuestContext.hpp"
#include "CPlayer.hpp"

CQuestContext::CQuestContext(CQuest* quest)
{
    m_permeable = true;

    m_quest = quest;
    for(auto it : m_quest->getHandler())
    {
        std::cout << "Adding listener for \"" << it.second << "\": " << it.first << std::endl;

        if(m_functions.count(it.first) == 0)
            std::cout << "FATAL ERROR, Quest-handler does not exits!!\n";
        else
            add_listener(it.second, m_functions[it.first]);
    }
}

    
std::map<string, void (CContext::*)(std::string&, CPlayer* p)> CQuestContext::m_functions = {};
void CQuestContext::initializeFunctions()
{
    //Use id of state as identfier
    m_functions["1ticketverkaeufer"] = &CContext::h_ticketverkaeufer;
    m_functions["2ticketkauf"]       = &CContext::h_ticketverkauf;
    m_functions["3zum_gleis"]        = &CContext::h_zum_gleis;
}

// ***** HANDLER **** //
void CQuestContext::h_ticketverkaeufer(std::string& sIdentifier, CPlayer* p)
{
    if(sIdentifier.find("to ") == 0)
        sIdentifier.erase(0, 3);
    if(p->getRoom()->getID() != "bahnhof_toiletten")
        return;

    if(fuzzy::fuzzy_cmp("männer-toilette", sIdentifier) <= 0.2)
        p->appendPrint(m_quest->getSteps()["1ticketverkaeufer"]->solved());

    delete_listener("go", 0);
}


void CQuestContext::h_ticketverkauf(std::string& sIdentifier, CPlayer* p)
{
    if(sIdentifier == "ticket")
        p->appendPrint(m_quest->getSteps()["2ticketkauf"]->solved());
}

void CQuestContext::h_zum_gleis(std::string& sIdentifier, CPlayer* p)
{    
    if(sIdentifier.find("to ") == 0)
        sIdentifier.erase(0, 3);

    if(p->getRoom()->getID() != "bahnhof_gleise" || p->getItem_byID("ticket") == NULL)
        return;

    if(fuzzy::fuzzy_cmp("gleis 3", sIdentifier) <= 0.2)
        p->appendPrint(m_quest->getSteps()["3zum_gleis"]->solved());

    sIdentifier = "";
    p->appendPrint("Du siehst deinen Zug einfahren. Du bewegst dich auf ihn zu, zeigst dein Ticket, der Schaffner musstert dich kurz und lässt dich dann eintreten. Du suchst dir einen freien Platz, legst dein Bündel auf den sitz neben dich und schläfst ein...\n $ Nach einem scheinbar endlos langem schlaf wachst du wieder in deinem Abteil auf. Das Abteil ist leer. Leer bist auf einen geheimnisvollen Begleiter: Parsen.");

    p->setRoom(p->getWorld()->getRooms()["compartment-a"]);

    p->getContexts().erase(m_quest->getID());
}
    
