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

// ***** PARSER ***** //

vector<CContext::event> CGameContext::parser(string sInput, CPlayer* p)
{
    if(p->getID().find("programmer") == std::string::npos)
        return {std::make_pair("access_error", "")};

    std::cout << "gameParser: " << sInput << std::endl;
    std::regex reloadGame("reloadgame");                //Reload all players and m_world of game
    std::regex reloadPlayer("(reloadplayer) (.*)");     //Reload player (world + stats, iventory etc.)
    std::regex reloadWorlds("reloadWorld");             //Reload all worlds, but not players
    std::regex reloadWorld("(reloadworld) (.*)");       //Reload world of a player (not his stats)
    std::regex updatePlayers("updateplayers");          //Adds new players
    std::smatch m;

    if(std::regex_match(sInput, reloadGame))
        return {std::make_pair("reloadGame", "")};
    else if(std::regex_match(sInput, m, reloadPlayer))
        return {std::make_pair("reloadPlayer", m[2])};
    else if(std::regex_match(sInput, reloadWorlds))
        return {std::make_pair("reloadWorlds", "")};
    else if(std::regex_match(sInput, m, reloadWorld))
        return {std::make_pair("reloadWorld", m[2])};
    else if(std::regex_match(sInput, updatePlayers))
        return {std::make_pair("updatePlayers", "")};

    std::cout << "Done.\n";
    
    m_permeable = true;
    return {};
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
