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

    // *** Die komische Gruppe *** // 
    m_functions["1reden"] = &CContext::h_reden;
    // *** Besoffene Frau *** //
    m_functions["1besiege_besoffene_frau"] = &CContext::h_besiege_besoffene_frau;

    // *** Geld auftreiben *** //
    m_functions["1geldauftreiben"] = &CContext::h_geldauftreiben;
}

// ***** HANDLER **** //


// *** *** Zug nach Moskau *** *** //
void CQuestContext::h_ticketverkaeufer(std::string& sIdentifier, CPlayer* p)
{
    std::cout << "h_ticketverkaeufer: " << p->getRoom()->getID() << ", " << sIdentifier << std::endl;
    if(p->getRoom()->getID() != "bahnhof_toiletten")
        return;

    if(fuzzy::fuzzy_cmp("männer-toilette", sIdentifier) <= 0.2) {
        p->questSolved(m_quest->getID(), "1ticketverkaeufer");
        delete_listener("go", 0);
    }
}


void CQuestContext::h_ticketverkauf(std::string& sIdentifier, CPlayer* p)
{
    if(sIdentifier == "ticket")
        p->questSolved(m_quest->getID(), "2ticketkauf");
}

void CQuestContext::h_zum_gleis(std::string& sIdentifier, CPlayer* p)
{    
    std::string room = p->getObject(p->getRoom()->getExtits(), sIdentifier);

    if(room != "gleis3")
        return; 

    p->questSolved(m_quest->getID(), "3zum_gleis");

    p->appendPrint("Du siehst deinen Zug einfahren. Du bewegst dich auf ihn zu, zeigst dein Ticket, der Schaffner musstert dich kurz und lässt dich dann eintreten. Du suchst dir einen freien Platz, legst dein Bündel auf den sitz neben dich und schläfst ein...\n $ Nach einem scheinbar endlos langem schlaf wachst du wieder in deinem Abteil auf. Das Abteil ist leer. Leer bist auf einen geheimnisvollen Begleiter: Parsen.\n");

    p->setRoom(p->getWorld()->getRooms()["compartment-a"]);
    m_curPermeable = false;
    m_block = true;
    p->getContexts().erase(m_quest->getID());
    p->questSolved("tutorial", "1tutorial");
}

// *** *** Die komische Gruppe *** *** //
void CQuestContext::h_reden(std::string& sIdentifier, CPlayer* p)
{
    std::string character = p->getObject(p->getRoom()->getCharacters(),sIdentifier);
    if(character == "" || character.find("passant") == std::string::npos)
        return;

    CQuestStep* step = m_quest->getSteps()["1reden"];
    for(size_t i=0; i<step->getWhich().size(); i++) {
        if(step->getWhich()[i] == p->getWorld()->getCharacters()[character]->getID())
            return;
    }

    int num = (((int)character.back()-48)-1)/3;
    step->getWhich().push_back("passant" + std::to_string(num*3+1));
    step->getWhich().push_back("passant" + std::to_string(num*3+2));
    step->getWhich().push_back("passant" + std::to_string(num*3+3));
    step->incSucc(1);
    p->questSolved(m_quest->getID(), "1reden");

    //Change dialog of all "Passanten"
    if(step->getSolved() == true)
    {
        for(size_t i=1; i<=9; i++)
        {
            std::cout << "Changing dialog for: passant" << i << std::endl;
            p->getWorld()->getCharacters()["passant"+std::to_string(i)]->setDialog(p->getWorld()->dialogFactory("strangeGuysDialog2", p));
        }
        p->getContexts().erase(m_quest->getID());
    }
}
    
// *** *** Besoffene Frau *** *** //
void CQuestContext::h_besiege_besoffene_frau(std::string& sIdentifier, CPlayer* p)
{
    if(sIdentifier != "besoffene_frau")
        return;

    p->questSolved(m_quest->getID(), "1besiege_besoffene_frau");
    p->appendPrint("Du suchst in den Taschen der Frau und findest drei Schillinge.\n");
    p->throw_event("recieveMoney 3");
    p->getContexts().erase(m_quest->getID());
}

// *** *** GELD AUFTREIBEN *** *** //
void CQuestContext::h_geldauftreiben(std::string& sIdentifier, CPlayer* p)
{
    if(p->getStat("gold") + stoi(sIdentifier) < 10)
        return;

    if(m_quest->getActive() == true)
        p->appendPrint("Wundervoll, genug Geld, um das Ticket zu kaufen!\n");

    std::cout << "in h_geldauftreiben() \n";
    p->questSolved(m_quest->getID(), "1geldauftreiben");
    p->getContexts().erase(m_quest->getID());
}