#include "CGameContext.hpp"
#include "CGame.hpp"

CGameContext::CGameContext() 
{
    //set permeable
    m_permeable = true;
    m_curPermeable = m_permeable;
    
    //Set handlers
    add_listener("reload_game", &CContext::h_reloadGame);
    add_listener("reload_player", &CContext::h_reloadPlayer);
    add_listener("reload_worlds", &CContext::h_reloadWorlds);
    add_listener("reload_world", &CContext::h_reloadWorld);
    add_listener("update_players", &CContext::h_updatePlayers);
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

void CGameContext::error(CPlayer*p)
{
    m_permeable = true;
}
