#ifndef CATTACK_H
#define CATTACK_H

#include <iostream>
#include <map>

using std::string;

class CAttack
{
private:
    string m_sName;
    string m_sDescription;
    string m_sOutput;
    size_t m_power;

public:
    CAttack(string sName, string sDescription, string sOutput, size_t power);

    // *** GETTER *** // 
    string getName();
    string getDescription();
    string getOutput();
    size_t getPower();
};

#endif

 
