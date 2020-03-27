#include "CTradeContext.hpp"
#include "CPlayer.hpp"
#include "CCharacter.hpp"

CTradeContext::CTradeContext(CPlayer* p, std::string partner)
{
    m_permeable=false;
    m_curPermeable = m_permeable;

    m_tradingPartner = p->getWorld()->getCharacters()[partner];
    m_collum=0;
    m_inventory = 0;

    m_sHelp = "trade.txt";

    h_help(m_sHelp, p); 
    print(p);
}


void CTradeContext::h_chooseSell(std::string& sIdentifier, CPlayer* p)
{    
    m_curItem = p->getInventory().getItem(m_collum, stoi(sIdentifier)-1);
    p->appendPrint("Sell " + m_curItem->getName() + "? (yes/no)\n");
    add_listener("yes", &CContext::h_decideSell, 0);
    add_listener("no", &CContext::h_decideSell, 0);
}

void CTradeContext::h_chooseBuy(std::string& sIdentifier, CPlayer* p)
{
    m_curItem = m_tradingPartner->getInventory().getItem(m_collum, stoi(sIdentifier)-1);
    p->appendPrint("Buy " + m_curItem->getName() + "? (yes/no)\n");
    add_listener("yes", &CContext::h_decideBuy, 0);
    add_listener("no", &CContext::h_decideBuy, 0);
}

void CTradeContext::h_decideSell(std::string& sIdentifier, CPlayer* p)
{
    if(sIdentifier == "yes")
    {
        p->appendPrint("PERZEPTION: Du hast " + m_curItem->getName() + " verkauft.\n\n");
        p->throw_event("recieveMoney " + std::to_string(m_curItem->getValue()));
        CItem* item = new CItem(m_curItem->getAttributes(), p);
        std::cout << "VALUE: " << item->getValue() << std::endl;
        m_tradingPartner->getInventory().addItem(item);
        p->getInventory().removeItemByID(m_curItem->getID());
    }

    else
        p->appendPrint("PERZEPTION: Du hast dich entschieden " + m_curItem->getName() + " nicht zu verkaufen.\n\n");

    print(p);
}

void CTradeContext::h_decideBuy(std::string& sIdentifier, CPlayer* p)
{
    if(sIdentifier == "yes")
    {
	if(p->getStat("gold") < m_curItem->getValue())
	{
	    p->appendPrint("PERZEPTION: Du hast nicht genuegend Geld um "+m_curItem->getName()+" zu kaufen");
	    return;
	}
        p->appendPrint("PERZEPTION: Du hast " + m_curItem->getName() + " gekauft.\n\n");
        p->setStat("gold", p->getStat("gold") - m_curItem->getValue());
        p->getInventory().addItem(new CItem(m_curItem->getAttributes(), p));
        m_tradingPartner->getInventory().removeItemByID(m_curItem->getID());
    }

    else
        p->appendPrint("PERZEPTION: Du hast dich entschieden " + m_curItem->getName() + " nicht zu kaufen.\n\n");

    print(p);
}

void CTradeContext::h_sell(std::string& sIdentifier, CPlayer* p)
{
    m_curItem = p->getInventory().getItem(sIdentifier);
    if(m_curItem == NULL)
        p->appendPrint("LOGIK: Dieser Gegenstand befindet sich nicht in deinem Inventar, du kannst ihn logischerwiese dehalb nicht verkaufen!\n");
    else
    {
        std::string sYes = "yes";
        h_decideSell(sYes, p);
    }
}

void CTradeContext::h_buy(std::string& sIdentifier, CPlayer* p)
{
    m_curItem = m_tradingPartner->getInventory().getItem(sIdentifier);
    if(m_curItem == NULL)
        p->appendPrint("LOGIK: Dieser Gegenstand befindet sich nicht in dem Inventar des Händlers, du kannst ihn logischerwiese dehalb nicht kaufen!\n");
    else
    {
        std::string sYes = "yes";
        h_decideBuy(sYes, p);
    }
}

void CTradeContext::h_switchLeft(std::string&, CPlayer* p)
{
    if(m_collum > 0)
        m_collum--;
    print(p);
}
void CTradeContext::h_switchRight(std::string&, CPlayer* p)
{
    m_collum++;
    print(p);
}
void CTradeContext::h_switchInventory(std::string&, CPlayer* p)
{
    m_inventory = (m_inventory+1)%2;
    m_collum=0;
    print(p);
}
void CTradeContext::h_exit(std::string&, CPlayer* p)
{
    p->getContexts().erase("trade");
    p->getContexts().erase("dialog");
    p->startDialog(m_tradingPartner->getID());
}

void CTradeContext::print(CPlayer* p)
{
    int aCollum = -1;
    int bCollum = -1;
    if(m_inventory == 0) aCollum = m_collum;
    else                 bCollum = m_collum;
        
    p->appendPrint(p->getName() + "'s Inventory:\n" + p->getInventory().printInventory("", aCollum));
    p->appendPrint("\n" + m_tradingPartner->getName() + "'s Inventory: \n" + m_tradingPartner->getInventory().printInventory("", bCollum));

    updateListeners(p);
}

void CTradeContext::updateListeners(CPlayer* p)
{
    m_eventmanager.clear();
    size_t num;
    if(m_inventory == 0)
    {
        if(p->getInventory().empty() == false) 
        {
            num = p->getInventory().getNumRomsInColum(m_collum);
            for(size_t i=1; i<=num; i++)
                add_listener(std::to_string(i), &CContext::h_chooseSell);
        }
    }
    else
    {
        num = m_tradingPartner->getInventory().getNumRomsInColum(m_collum);
        for(size_t i=1; i<=num; i++)
            add_listener(std::to_string(i), &CContext::h_chooseBuy);
    }

    add_listener("l", &CContext::h_switchLeft);
    add_listener("r", &CContext::h_switchRight);
    add_listener("i", &CContext::h_switchInventory);
    add_listener("sell", &CContext::h_sell);
    add_listener("buy", &CContext::h_buy);
    add_listener("exit", &CContext::h_exit);
}

void CTradeContext::error(CPlayer* p)
{
    h_help(m_sHelp, p);
}
