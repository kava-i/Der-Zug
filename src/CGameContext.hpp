#ifndef CGAMECONTEXT_H
#define CGAMECONTEXT_H

#include "CContext.hpp"


class CGameContext : public CContext
{
private:
    CGame* m_game;

public: 
    CGameContext() 
    {
        //set permeable
        m_permeable = true;
        
        //Set handlers
        add_listener("reloadGame", &CContext::h_reloadGame);
        add_listener("reloadPlayer", &CContext::h_reloadPlayer);
        add_listener("reloadWorlds", &CContext::h_reloadWorlds);
        add_listener("reloadWorld", &CContext::h_reloadWorld);
        add_listener("updatePlayers", &CContext::h_updatePlayers);
        add_listener("access_error", &CContext::h_accessError);
    }
    
    //Set Game
    void setGame(CGame* game);

    //Parser
    vector<event> parser(string, CPlayer* p);

    //Handlers
    void h_reloadGame(string&, CPlayer*);
    void h_reloadPlayer(string&, CPlayer*);
    void h_reloadWorlds(string&, CPlayer*);
    void h_reloadWorld(string&, CPlayer*);
    void h_updatePlayers(string&, CPlayer*);
    void h_accessError(string&, CPlayer*);
};

#endif
