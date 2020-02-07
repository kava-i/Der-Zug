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
    bool m_solved;
    bool m_active;
    std::map<std::string, CQuestStep*> m_questSteps;
    std::map<std::string, std::string> m_handler;

public:
    CQuest(nlohmann::json jAttributes);

    //Getter 
    std::string getID();
    bool getActive();
    std::map<std::string, CQuestStep*> getSteps();
    std::map<std::string, std::string> getHandler();

    //Setter
    void setSteps(std::map<std::string, CQuestStep*> steps);
    void setHandler(std::map<std::string, std::string> handlers);

    std::string setActive();

    //Functions
    std::string printQuest(bool solved);
    void checkSolved();
};

class CQuestStep
{
private:
    std::string m_sName, m_sID, m_sDescription;
    bool m_solved, m_active;
    
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

    //Setter
    void setActive(bool);

    //Functions
    std::string solved();
    std::string handleSolved();
};

#endif

