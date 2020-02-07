#include "CQuestContext.hpp"
#include "CPlayer.hpp"

CQuestContext::CQuestContext(CQuest* quest)
{
    //Set permeability
    m_permeable = true;
    m_curPermeable = m_permeable;

    //Create hendlers based on handlers set for this quest
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

    // *** Zug nach Moskau *** //
    m_functions["1ticketverkaeufer"] = &CContext::h_ticketverkaeufer;
    m_functions["2ticketkauf"]       = &CContext::h_ticketverkauf;
    m_functions["3zum_gleis"]        = &CContext::h_zum_gleis;

    // *** Besoffene Frau *** //
    m_functions["1besiege_besoffene_frau"] = &CContext::h_besiege_besoffene_frau;

    // *** Geld auftreiben *** //
    m_functions["1geldauftreiben"] = &CContext::h_geldauftreiben;
}

// ***** HANDLER **** //


// *** *** Zug nach Moskau *** *** //
void CQuestContext::h_ticketverkaeufer(std::string& sIdentifier, CPlayer* p)
{
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
    if(p->getRoom()->getID() != "bahnhof_gleise" || p->getItem_byID("ticket") == NULL)
        return;

    if(fuzzy::fuzzy_cmp("gleis 3", sIdentifier) <= 0.2)
        p->appendPrint(m_quest->getSteps()["3zum_gleis"]->solved());

    p->appendPrint("Du siehst deinen Zug einfahren. Du bewegst dich auf ihn zu, zeigst dein Ticket, der Schaffner musstert dich kurz und lässt dich dann eintreten. Du suchst dir einen freien Platz, legst dein Bündel auf den sitz neben dich und schläfst ein...\n $ Nach einem scheinbar endlos langem schlaf wachst du wieder in deinem Abteil auf. Das Abteil ist leer. Leer bist auf einen geheimnisvollen Begleiter: Parsen.");

    p->setRoom(p->getWorld()->getRooms()["compartment-a"]);
    m_curPermeable = false;
    m_block = true;
    p->getContexts().erase(m_quest->getID());
}
    
// *** *** Besoffene Frau *** *** //
void CQuestContext::h_besiege_besoffene_frau(std::string& sIdentifier, CPlayer* p)
{
    if(sIdentifier != "besoffene_frau")
        return;

    p->appendPrint(m_quest->getSteps()["1besiege_besoffene_frau"]->solved());
    
    p->appendPrint("Du suchst in den Taschen der Frau und findest drei Schillinge.\n");
    p->throw_event("recieveMoney 3");
    p->getContexts().erase(m_quest->getID());
}

// *** *** GELD AUFTREIBEN *** *** //
void CQuestContext::h_geldauftreiben(std::string& sIdentifier, CPlayer* p)
{
    if(p->getGold() + stoi(sIdentifier) < 10)
        return;

    if(m_quest->getActive() == true)
        p->appendPrint("Wundervoll, genug Geld, um das Ticket zu kaufen!\n");

    p->appendPrint(m_quest->getSteps()["1geldauftreiben"]->solved());
    p->getContexts().erase(m_quest->getID());
}
