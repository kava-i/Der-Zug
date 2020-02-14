#ifndef CCHARACTER_H
#define CCHARACTER_H

#include <iostream>
#include <string>
#include <map>
#include "CPerson.hpp"
#include "CDialog.hpp"
#include "CText.hpp"

using std::string;
using std::map;

class CPlayer;

class CCharacter : public CPerson
{ 
private:
    CText* m_text;
    string m_sDescription;

public:
    CCharacter(nlohmann::json jAtts, SDialog* newDialog, attacks newAttacks, CPlayer* p);

    // *** GETTER *** // 
    string getDescription();

};
    
#endif
