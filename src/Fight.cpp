#include "CFight.hpp"

CFight::CFight(CPerson* player, CPerson* opponent)
{
    m_sName = "Fight";
    m_sDescription = "Fighting against " + opponent->getName();
    m_player = player;
    m_opponent = opponent;
}

CPerson* CFight::getOpponent() { return m_opponent; }

void CFight::initializeFight()
{
    m_player->appendPrint(m_sDescription + createFightingAgainst());
}

string CFight::fightRound(string sPlayerChoice)
{
    string sOutput;
    //Execute players turn
    sOutput += turn(m_player->getAttack(sPlayerChoice), m_player, m_opponent) + "$\n";

    //Check wether opponent is dead
    if(m_opponent->getStat("hp") <= 0) {
        sOutput += "Du hast " + m_opponent->getName() + " besiegt!\n";
        m_player->appendPrint(sOutput);
        int ep = m_opponent->getStat("ep");
        if(ep != 0) {
            m_player->appendSuccPrint("+ " + std::to_string(ep) + " EP\n");
            m_player->addEP(ep);
        }
        return "endFight;finishCharacter " + m_opponent->getID();
    }
    sOutput+=createFightingAgainst();

    //Execute opponents turn
    string sAttack = pickOpponentAttack();
    sOutput += turn(sAttack, m_opponent, m_player) + "$\n";

    //Check wether player is dead
    if(m_player->getStat("hp") <= 0) {
        sOutput += "Du wurdest von " + m_opponent->getName() + " getötet!\n";
        m_player->appendPrint(sOutput);
        return "endFight;gameover";
    }

    sOutput+=createFightingAgainst();
    //Update player print
    m_player->appendPrint(sOutput);

    return "";
}

string CFight::turn(string selectedAttack, CPerson* attacker, CPerson* defender)
{
    //Get selected attack 
    CAttack* attack = attacker->getAttacks()[selectedAttack];

    //Create output
    string sOutput; 
    sOutput += "\n<div style=\"display: flex;justify-content: center;\">";
    sOutput += attacker->getName() + " benutzt " + attack->getName() + "\n";
    sOutput += attack->getOutput() + "\n";

    int damage = attack->getPower() + attacker->getStat("strength");
    sOutput += "Angerichteter Schaden: " + std::to_string(damage) + "\n";
    sOutput += "</div>";
    defender->setStat("hp", defender->getStat("hp") - damage);


    return sOutput;
}


string CFight::printStats(CPerson* person,bool printDesc)
{
    int hp = person->getStat("hp");
    int max_hp = person->getStat("max_hp");
    std::string sOutput = "<div style=\"padding-right: 3rem;\"><span style=\"color:green;padding-right:1rem\">"+person->getName()+"</span><div style=\"position:relative;left:0;top:0;display:inline-block;width:10rem;height:1rem;\"><div style=\"height:100%;background-color: #E74C3C;width:"+std::to_string((int)hp*100/max_hp)+"%;\"></div><div style=\"position: absolute;top:0;left:0;right:0;color:white;text-align: center;\">HP:"+std::to_string(hp)+"/"+std::to_string(max_hp)+"</div></div> <span style=\"padding-left: 1rem;color:#2e86c1;\">Strength: " + std::to_string(person->getStat("strength")) + "</span>\n";

    sOutput += "\n";
    if(printDesc)
    {
	    sOutput+=person->getReducedDescription();
	    sOutput+="\n\n";
    }
    else
        sOutput += person->printAttacksFight();

    sOutput += "</div>";
    return sOutput;
}

string CFight::createFightingAgainst() 
{
    std::string sOutput;
    sOutput += "\n<div style=\"display: flex;justify-content: center;\">";

    std::string kk = printStats(m_player,false);
    sOutput+=kk;
    sOutput += "<span style='padding-right: 3rem;'>";
    for(int i = 0; i <  std::count(kk.begin(), kk.end(), '\n');i++)
    {
	    sOutput+="|\n";
    }
    sOutput += "</span>";
    
    sOutput += printStats(m_opponent);

    sOutput += "</div>";
    
    return sOutput;
}

string CFight::pickOpponentAttack()
{
   srand(time(NULL));
   size_t attack = rand() % m_opponent->getAttacks().size() + 1;
   return m_opponent->getAttack(std::to_string(attack));
}

