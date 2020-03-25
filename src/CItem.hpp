#ifndef CITEM_H
#define CITEM_H

#include <iostream>
#include <map>
#include "CObject.hpp"
#include "json.hpp"
#include "func.hpp"

class CPlayer;

using std::string;

class CItem : public CObject
{
protected:

    /**
    * Json with all attributes of player. This allows the item to have additional 
    * attributes, only one particular item has. Also it allows creating items
    * from basic-items, as seen in the constructor.
    */
    nlohmann::json m_jAtts; 

    ///Static map f all state-functions
    static std::map<string, void (CItem::*)(CPlayer*)> m_functions;

public:

    /**
    * Constructor for non-derived items simply placing the passed json as items attributes.
    * @param[in] basic attributes from base item.
    */
    CItem(nlohmann::json jBasic) : CObject{jBasic}, m_jAtts{jBasic} {}

    /**
    * Constructor for derived items. The basic json usually supplies basic attributes. Which 
    * might get overridden, by specific attributes, which otherwise only supply extra item
    * specific attributes.
    * @param[in] jBasic basic attributes from base item.
    * @param[in] jItem extra attributes from specific individual item.
    */
    CItem(nlohmann::json jBasic, nlohmann::json jItem, std::string sID="") : CObject {jBasic} 
    {
        m_jAtts = jBasic;

        for(auto it=jItem.begin(); it!=jItem.end(); ++it) {
            if(it.key() == "from" && it.key() == "amount")
                continue;
            m_jAtts[it.key()] = it.value();
        }

        m_jAtts["id"] = sID;
        m_sID = sID;
        m_sName = m_jAtts.value("name", "");
    }

    // *** GETTER *** // 
    template <typename T> T getAttribute(std::string sName)
    {
        if(m_jAtts.count(sName) == 0)
            return T();
        return m_jAtts[sName].get<T>();
    }

    nlohmann::json getAttributes();
    string getDescription();
    string getFunction();
    size_t getEffekt();
    int getValue();
    
    // *** SETTER *** //
    template <typename T> void setAttribute(std::string sName, T t1)
    {
        m_jAtts[sName] = t1;
    }

    static void initializeFunctions();
    bool callFunction(CPlayer* p);

    // *** FUNCTIONS *** //
    void equipeWeapon(CPlayer*);
    void consumeDrug(CPlayer*);
};

#endif

 
