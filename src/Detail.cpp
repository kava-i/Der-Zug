#include "CDetail.hpp"

CDetail::CDetail(string sName, string sID, string sDescription, string sLook, objectmap characters, objectmap items)
{
    m_sName = sName;
    m_sID = sID;
    m_sDescription = sDescription;
    m_sLook = sLook;
    m_characters = characters;
    m_items = items;
}

// *** GETTER *** //
string CDetail::getName() {
    return m_sName;
}

string CDetail::getID() {
    return m_sID;
}

string CDetail::getDescription() {
    return m_sDescription;
}

string CDetail::getLook() { 
    return m_sLook; 
}

CDetail::objectmap& CDetail::getCharacters() { 
    return m_characters; 
}

CDetail::objectmap& CDetail::getItems() { 
    return m_items; 
}
 

// *** FUNCTIONS *** //
string CDetail::look(string sWhere, string sWhat) {
    return "";
}
