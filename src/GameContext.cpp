#include "CGameContext.hpp"
#include "CGame.hpp"


// ***** SETTER ***** //
void CGameContext::setGame(CGame* game) {
    m_game = game;
}

// ***** PARSER ***** //

vector<event> CGameContext::parser(string sInput, CPlayer* p)
{
    std::regex reloadGame("game_reloadGame()");
    std::regex reloadPlayer("(game_reloadPlayers()) (.*)");
    std::regex reloadWorld("game_reloadWorld) (.*)");
    std::regex updatePlayers("game_updatePlayers()");
    std::smatch m;
    
    if(std::regex_match(sInput, reloadGame))
        return {std::make_pair("reloadGame", "")};
    else if(std::regex_match(sInput, m, reloadPlayer))
        return {std::make_pair("reloadPlayer", m[2])};
    else if(std::regex_match(sInput, m, reloadWorld)
        return {std::make_pair("reloadWorld", m[2])};
    else if(std::regex_match(sInput, updatePlayers))
        return {std::make_pair("updatePlayers", "")};
    
    else {
        m_permeable = true;
        return {}
    }
}



     
// ***** HANDLER ***** 

void CGameContext::h_reloadGame(string&, CPlayer*)
{
    std::cout << "reloading game... \n";
}

void CGameContext::h_reloadPlayer(string& sPlayer, CPlayer*)
{
    std::cout << "reloading Player: " << sPlayer << "... \n";
}

void CGameContext::h_reloadWorld(string& sPlayer, CPlayer*)
{
    std::cout << "world from player \"" << sPlayer << "\"... \n";
}

void CGameContext::h_updatePlayers(string&, CPlayer*)
{
    std::cout << "updating players... \n",
}
