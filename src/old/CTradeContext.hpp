#ifndef CTRADECONTEXT_H
#define CTRADECONTEXT_H

#include "CContext.hpp"
class CPerson;
class CItem;

class CTradeContext : public CContext
{
private: 
    CPerson* m_tradingPartner;
    size_t m_collum;
    size_t m_inventory;
    CItem* m_curItem;

public:
    CTradeContext(CPlayer* p, std::string partner);

    void h_chooseSell(std::string&, CPlayer*);
    void h_chooseBuy(std::string&, CPlayer*);
    void h_decideSell(std::string&, CPlayer*);
    void h_decideBuy(std::string&, CPlayer*);
    void h_sell(std::string&, CPlayer*);
    void h_buy(std::string&, CPlayer*);
    void h_switchLeft(std::string&, CPlayer*);
    void h_switchRight(std::string&, CPlayer*);
    void h_switchInventory(std::string&, CPlayer*);
    void h_exit(std::string&, CPlayer*);
    
    void print(CPlayer* p);
    void updateListeners(CPlayer* p);
    void error(CPlayer*);
};

#endif
