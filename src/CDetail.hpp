#ifndef CDETAILS_H
#define CDETAILS_H

#include <iostream>
#include <map>
#include <string>

using std::string;


class CDetail
{
private: 
    string m_sName;
    string m_sID;
    string m_sDescription;

    string m_sLook;

    typedef std::map<string, string> objectmap;
    objectmap m_characters;
    objectmap m_items;

public:

    //Constructor 
    CDetail(string sName, string sID, string sDescription, string look, objectmap characters, objectmap items);

    // *** GETTER *** //
    string getName();
    string getID();
    string getDescription();
    string getLook();
    objectmap& getCharacters();
    objectmap& getItems();

    // *** FUNCTIONS *** //
    string look(string sWhere, string sWhat);

};

#endif
