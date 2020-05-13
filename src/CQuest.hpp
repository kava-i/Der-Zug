#ifndef CQUEST_H
#define CQUEST_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "json.hpp"
#include "Webcmd.hpp"

class CQuestStep;

class CQuest {
private:
    std::string m_sName;
    std::string m_sID;
    std::string m_sDescription;
    int m_EP;
    bool m_solved;
    bool m_active;
    std::map<std::string, CQuestStep*> m_questSteps;
    std::vector<nlohmann::json> m_handler;

public:
    CQuest(nlohmann::json jAttributes);

    //Getter 
    std::string getID();
    bool getActive();
    std::map<std::string, CQuestStep*> getSteps();
    std::vector<nlohmann::json> getHandler();

    //Setter
    void setSteps(std::map<std::string, CQuestStep*> steps);
    void setHandler(std::vector<nlohmann::json> handlers);

    std::string setActive(int& ep);

    //Functions
    std::string printQuest(bool solved);
    std::string checkSolved(int& ep);
};

class CQuestStep
{
private:
    std::string m_sName, m_sID, m_sDescription;
    bool m_solved, m_active;

    int m_succ;
    int m_curSucc;
    std::vector<std::string> m_which;
    
    std::vector<std::string> m_linkedSteps;

    //Link to quest
    CQuest* m_quest;
 
public: 
    CQuestStep(nlohmann::json jAttributes, CQuest* quest);

    //Getter
    std::string getName();
    std::string getDescription();
    bool getActive();
    bool getSolved();
    int getSucc();
    int getCurSucc();
    std::vector<std::string>& getWhich();

    //Setter
    void setActive(bool);
    void setCurSucc(int x);
    void incSucc(int x);

    //Functions
    std::string solved(int& ep);
    std::string handleSolved();
};

#endif

