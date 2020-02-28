#ifndef CTRADECONTEXT_H
#define CTRADECONTEXT_H

#include "CContext.hpp"

class CTradeContext : public CContext
{
private: 
    string m_curState;
    std::string m_tradingPartner;
    size_t collum;
    bool inventory;

public:
    CTradeContext(CPlayer* p, std::string partner);

    void error(CPlayer*);
};

#endif
