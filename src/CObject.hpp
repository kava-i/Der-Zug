#ifndef COBJECT_H
#define COBJECT_H

#include <iostream>
#include <map>
#include "CText.hpp"
#include "json.hpp"

class CPlayer;

/**
* Class from which every other object derives, as every object has at least 
* a name and an id. Later maybe information on grammar will be added.
*/
class CObject 
{
protected: 
    std::string m_sID;      ///< unique identifier for object.
    std::string m_sName;    ///< name of object.
    CText* m_text;          ///< text class holdes description

public:

    /**
    * Constructor.
    * @param[in] jAttributes json containing all attributes of object.
    */
    CObject(nlohmann::json jAttributes, CPlayer* p)
    {
        m_sID = jAttributes.value("id", "");
        m_sName = jAttributes.value("name", "");
        m_text = new CText(jAttributes.value("description", nlohmann::json::parse("{}") ), p);
    }

    virtual ~CObject() { delete m_text; }

    // *** GETTER *** //

    ///Return id of object
    std::string getID() {
        return m_sID;
    }

    ///Return name of object.
    std::string getName() {
        return m_sName;
    }

    ///Return description by calling print-function from text class.
    std::string getDescription() {
        return m_text->print();
    }

    ///Set id of object.
    void setID(std::string sID) {
        m_sID = sID;
    }

    ///Set name of object.
    void setName(std::string sName) {
        m_sName = sName;
    }
};

#endif
