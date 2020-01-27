#include "CAttack.hpp"

CAttack::CAttack(string sName, string sDescription, string sOutput, size_t power)
{
    m_sName = sName;
    m_sDescription = sDescription;
    m_sOutput = sOutput;
    m_power = power; 
}

// *** GETTER *** //
string CAttack::getName()       { return m_sName; }
string CAttack::getDescription(){ return m_sDescription; }
string CAttack::getOutput()     { return m_sOutput; }
size_t CAttack::getPower()      { return m_power; }
