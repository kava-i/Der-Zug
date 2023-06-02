#ifndef CQUEST_H
#define CQUEST_H

#include <iostream>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <vector>
#include "objects/helper/update.h"
#include "tools/webcmd.h"

class CQuestStep;
class CPlayer;
class CListener;

class CQuest {
private:
    std::string m_sName;
    std::string m_sID;
    std::string m_sDescription;
		Updates updates_;
    bool m_solved;
    bool m_active;
    bool m_onlineFromBeginning;
    bool silent_;
    std::vector<std::string> m_first_active_steps;
    std::map<std::string, CQuestStep*> m_questSteps;
    std::vector<CListener*> lisneters_;

public:
    CQuest(nlohmann::json jAttributes);

    //Getter 
    std::string getID();
    bool getSolved();
    bool getActive();
    bool getOnlineFromBeginning();
    std::map<std::string, CQuestStep*> getSteps();
    std::vector<CListener*> listeners();
		const Updates& updates();
		bool silent() const;

    // Setter
    void setSteps(std::map<std::string, CQuestStep*> steps);
    void setListeners(std::vector<CListener*> handlers);

    std::string setActive(CPlayer* p);

    // Functions
    std::string printQuest(bool solved);
    std::string checkSolved();
};

class CQuestStep
{
private:
    std::string m_sName; 
    std::string m_sID;
    std::string m_sDescription;
    bool m_solved, m_active;

    int m_succ;
    int m_curSucc;
    std::vector<std::string> m_which;
    
    std::vector<std::string> m_linkedSteps;

    std::string m_events;
    std::string m_preEvents;
    std::map<std::string,std::string> m_info;
    std::string logic_;
    nlohmann::json updates_;

    //Link to quest
    CQuest* m_quest;
 
public: 
    CQuestStep(nlohmann::json jAttributes, CQuest* quest);

    //Getter
    std::string getID();
    std::string getName();
    std::string getDescription();
    bool getActive();
    bool getSolved();
    int getSucc();
    int getCurSucc();
    std::vector<std::string>& getWhich();
    std::vector<std::string>& getLinkedSteps();
    std::string getEvents();
    std::string getPreEvents();
    std::map<std::string, std::string> getInfo();
    std::string logic();

    //Setter
    void setActive(bool, CPlayer*);
    void setCurSucc(int x);
    void incSucc(int x);

    //Functions
    void solved(CPlayer*);
    std::string UpdateAttributes(CPlayer* p);
};

#endif

