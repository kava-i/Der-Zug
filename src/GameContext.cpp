#include "CGameContext.hpp"
#include "CGame.hpp"

CGameContext::CGameContext() 
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
 
// ***** SETTER ***** //
void CGameContext::setGame(CGame* game) {
    m_game = game;
}

// ***** HANDLER ***** 

void CGameContext::h_reloadGame(string&, CPlayer* p)
{
    p->appendPrint("reloading game... \n");
    m_permeable = false;
}

void CGameContext::h_reloadPlayer(string& sPlayer, CPlayer*p)
{
    p->appendPrint("reloading Player: " + sPlayer + "... \n");
    if(m_game->reloadPlayer(sPlayer) == false)
        p->appendPrint("Player does not exist... reloading world failed.\n");
    else
        p->appendPrint("Done.\n");
    m_permeable = false;
}

void CGameContext::h_reloadWorlds(string& sPlayer, CPlayer*p)
{
    p->appendPrint("reloading all worlds... \n");
    if(m_game->reloadWorld() == true)
        p->appendPrint("Reloading all worlds failed.\n");
    else
        p->appendPrint("Done.\n");
    m_permeable = false;
}

void CGameContext::h_reloadWorld(string& sPlayer, CPlayer*p)
{
    p->appendPrint("reloading world of: " + sPlayer + "... \n");
    if(m_game->reloadWorld(sPlayer) == false)
        p->appendPrint("Player does not exist... reloading world failed.\n");
    else
        p->appendPrint("Done.\n"); 
    m_permeable = false;
}

void CGameContext::h_updatePlayers(string&, CPlayer*p)
{
    p->appendPrint("updating players... \n");
    m_game->playerFactory(true); 
    p->appendPrint("done.\n");
    m_permeable = false;
}

void CGameContext::h_accessError(string&, CPlayer*p)
{
    p->appendPrint("You have no permission to call these functions!!\n");
    m_permeable = false;
}

void CGameContext::error(CPlayer*p)
{
    m_permeable = true;
}
