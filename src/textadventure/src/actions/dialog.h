#ifndef CDIALOG_H
#define CDIALOG_H

#include <iostream>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <vector>

#include "concepts/text.h"

using std::string;
using std::map;
using std::vector;

class CDState;
class CPlayer;

class CDialog 
{
private: 
    std::string m_sName;
    std::map<std::string, CDState*> m_states;
    std::string music_;
    std::string image_;

public:
    CDialog() {}
    CDialog(std::string sName, std::map<std::string, CDState*> states);

    ///Getter for name of dialog.
    std::string getName();

    ///Getter for map of states in dialog.
    std::map<std::string, CDState*>& getStates();

    ///Getter for a single state
    CDState* getState(std::string sID);

    ///Setter for dialog name.
    void setName(std::string sName);
    
    ///Setter for dialog states.
    void setStates(std::map<std::string, CDState*> states);


    // *** various functions *** //
    
    std::string visited();

    ///Function changing the text of a state.
    void changeStateText(string sStateID, size_t text);

    //Function adding a new dialog option to a given state.
    void addDialogOption(string sStateID, size_t optID);

    ///Function deleting an option from a given state.
    void deleteDialogOption(string sStateID, size_t optID);

    ///Function moveing given state to the begining of the dialog
    void moveStart(std::string sStateID);

    ///Function changing the complete dialog.
    void changeDialog(string sOld, string sNew, CPlayer* p);


};

struct SDOption {
    string sText;
    string logic_;
    string sTarget;
    bool visited;
};

class CDState {
private:

    CText* m_text;  //< Text which will be printed after call.
    string m_sFunction;  //< Function called when calling state.
    string m_sActions;  //< Actions executed after call.
    string m_sEvents;  //< string holding a list of events, which will be executed.
    std::string music_;
    std::string image_;

    bool visited_;

    //Map of options 
    typedef std::map<int, SDOption> dialogoptions;
    dialogoptions m_options;

    //Reference to the complete Dialoge
    CDialog* m_dialog;

    //Static map f all state-functions
    static std::map<string, string (CDState::*)(CPlayer*)> m_functions;

public:
    CDState(nlohmann::json jAtts, dialogoptions states, CDialog* dia, CPlayer* p);

    // *** GETTER *** // 
    string getText();
    dialogoptions& getOptions();
    bool visited();
    std::string music();
    std::string image();

    // *** SETTER *** //
    void set_music(std::string file_name);

    static void initializeFunctions();
    string callState(CPlayer*);
    string getNextState(string sPlayerChoice, CPlayer* p);
    void executeActions(CPlayer* P);


    // *** functions *** //
    string toeten(CPlayer*);
    string standard(CPlayer*);
    string keinTicket(CPlayer*);
    string strangeGuy1(CPlayer*); 
    string strangeGuy2(CPlayer*); 


    std::vector<size_t> getActiveOptions(CPlayer*);
    int numOptions();
};
#endif 
