#ifndef CFIGHTCONTEXT_H
#define CFIGHTCONTEXT_H

#include "CContext.hpp"

class CFightContext : public CContext
{
public: 
    CFightContext();

    //Parser
    vector<event> parser(string, CPlayer*);

    //Handlers
    void h_choose(string&, CPlayer*);
    void h_show(string&, CPlayer*);
    void h_error(string&, CPlayer*);

};

#endif
