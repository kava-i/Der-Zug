#ifndef CTEXT_H
#define CTEXT_H

#include <iostream>
#include <vector>
#include "json.hpp"
#include "func.hpp"
#include "Webcmd.hpp"
#include "CMind.hpp"

class CPlayer;
class COutput;

class CText
{
private:
    CPlayer* m_player;
    std::vector<COutput*> m_texts;

public:
    CText(nlohmann::json jAttributes, CPlayer*);

    std::string print(bool events=true);
    std::string reducedPrint(bool events=true);
};

class COutput
{
private:
    std::string m_sSpeaker;
    std::string m_sText;

    nlohmann::json m_jDeps;
    nlohmann::json m_jUpdates;
    std::vector<std::string> m_permanentEvents;
    std::vector<std::string> m_oneTimeEvents;

public:
    COutput(nlohmann::json jAttributes, CPlayer*);

    // *** getter *** //
    std::string getText();
    std::string getSpeaker();

    std::string print(CPlayer* p, bool events=true);
    std::string reducedPrint(CPlayer* p, bool events=false);

    bool checkDependencies(std::string& sSuccess, CPlayer* p);
    void updateAttrbutes(std::string& sUpdated, CPlayer* p);
    void addEvents(CPlayer* p);
};

#endif
    
