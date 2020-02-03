#ifndef CQUEST_H
#define CQUEST_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "json.hpp"

class CQuestStep;

class CQuest {
private:
    std::string m_sName;
    std::string m_sID;
    std::string m_sDescription;
    bool m_solved;
    bool m_active;
    std::map<std::string, CQuestStep*> m_questSteps;

public:
    CQuest(nlohmann::json jAttributes);

    //Getter 
    bool getActive();
    std::map<std::string, CQuestStep*> getSteps();

    //Setter
    void setSteps(std::map<std::string, CQuestStep*> steps);

    //Functions
    std::string printQuest();
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

    //Setter
    void setActive(bool);

    //Functions
    void solved();
};

#endif

