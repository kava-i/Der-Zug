#ifndef CTEXT_H
#define CTEXT_H

#include <iostream>
#include "json.hpp"
#include "func.hpp"
#include "Webcmd.hpp"

class CPlayer;
class COutput;

class CText
{
private:
    CPlayer* m_player;
    std::vector<COutput*> m_texts;

public:
    CText(nlohmann::json jAttributes, CPlayer*);

    std::string print();
};

class COutput
{
private:
    std::string m_sSpeaker;
    std::string m_sText;
    nlohmann::json m_jDeps;
public:
    COutput(std::string sAtts);

    // *** getter *** //
    std::string getText();
    std::string getSpeaker();
    nlohmann::json getDeps();

    std::string print(CPlayer* p);
};

#endif
    
