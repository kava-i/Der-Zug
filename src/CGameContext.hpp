#ifndef CGAMECONTEXT_H
#define CGAMECONTEXT_H

#include "CContext.hpp"


class CGameContext : public CContext
{
private:
    CGame* m_game;

public: 
    CGameContext();
   
    //Set Game
    void setGame(CGame* game);

    //Handlers
    void h_reloadGame(string&, CPlayer*);
    void h_reloadPlayer(string&, CPlayer*);
    void h_reloadWorlds(string&, CPlayer*);
    void h_reloadWorld(string&, CPlayer*);
    void h_updatePlayers(string&, CPlayer*);

    void error(CPlayer*);
};

#endif
