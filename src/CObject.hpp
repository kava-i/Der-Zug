#ifndef COBJECT_H
#define COBJECT_H

#include <iostream>
#include <map>
#include "json.hpp"

/**
* Class from which every other object derives, as every object has at least 
* a name and an id. Later maybe information on grammar will be added.
*/
class CObject 
{
protected: 
    std::string m_sID;      ///< unique identifier for object.
    std::string m_sName;    ///< name of object.
public:

    /**
    * Constructor.
    * @param[in] jAttributes json containing all attributes of object.
    */
    CObject(nlohmann::json jAttributes)
    {
        m_sID = jAttributes.value("id", "");
        m_sName = jAttributes.value("name", "");
    }

    // *** GETTER *** //

    ///Return id of object
    std::string getID() {
        return m_sID;
    }

    ///Return name of object.
    std::string getName() {
        return m_sName;
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
