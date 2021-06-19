#ifndef CTEXT_H
#define CTEXT_H

#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

#include "tools/func.h"
#include "tools/webcmd.h"
#include "objects/mind.h"

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
    std::string pagePrint(size_t page);
    size_t getNumPages();
    bool underline(size_t page, std::string str1, std::string str2);
};

class COutput
{
private:
    std::string m_sSpeaker;
    std::string m_sText;
    std::string music_;
    std::string image_;

    std::string logic_;
    nlohmann::json m_jUpdates;
    std::vector<std::string> m_pre_permanentEvents;
    std::vector<std::string> m_pre_oneTimeEvents;
    std::vector<std::string> m_post_permanentEvents;
    std::vector<std::string> m_post_oneTimeEvents;

    //In case of book, or read-item
    size_t m_page;

public:
    COutput(nlohmann::json jAttributes);

    // *** getter *** //
    std::string getText();
    std::string getSpeaker();
    size_t getPage();

    std::string print(CPlayer* p, bool events=true);
    std::string reducedPrint(CPlayer* p, bool events=false);

    bool checkDependencies(std::string& sSuccess, CPlayer* p);
    void updateAttrbutes(CPlayer* p, std::string& sUpdated);
    void updateAttrbutes(CPlayer* p);
    void addEvents(CPlayer* p);

    bool underline(std::string str1, std::string str2);
};

#endif
    
