#ifndef CITEM_H
#define CITEM_H

#include <iostream>
#include <map>
#include "CObject.hpp"
#include "json.hpp"
#include "func.hpp"

class CPlayer;

class CItem : public CObject
{
protected:

    nlohmann::json m_jAttributes;
    std::string m_sType;
    std::string m_sFunction;
    std::string m_sAttack;
    size_t m_effekt;
    int m_value;
    bool m_hidden;

    ///Static map f all state-functions
    static std::map<std::string, void (CItem::*)(CPlayer*)> m_functions;

public:

    /**
    * Constructor for non-derived items simply placing the passed json as items attributes.
    * @param[in] basic attributes from base item.
    */
    CItem(nlohmann::json jBasic, CPlayer* p) : CObject{jBasic, p}
    {
        m_jAttributes = jBasic;
        m_sType = jBasic.value("type", "");
        m_sAttack = jBasic.value("attack", "");
        m_sFunction = jBasic.value("function", "");
        m_effekt = jBasic.value("effekt", 0);
        m_value = jBasic.value("value", 1);
        m_hidden = jBasic.value("value", false);
    }

    /**
    * Constructor for derived items. The basic json usually supplies basic attributes. Which 
    * might get overridden, by specific attributes, which otherwise only supply extra item
    * specific attributes.
    * @param[in] jBasic basic attributes from base item.
    * @param[in] jItem extra attributes from specific individual item.
    */
    CItem(nlohmann::json jBasic, nlohmann::json jItem, CPlayer* p, std::string sID="") : CObject {jBasic, p} 
    {
        m_sID = sID;
        m_sType = jItem.value("type", jBasic.value("type", ""));
        m_sAttack = jItem.value("attack", jBasic.value("attack", ""));
        m_sFunction = jItem.value("function", jBasic.value("function", ""));
        m_effekt = jItem.value("effekt", jBasic.value("effekt", 0));
        m_value = jItem.value("value", jBasic.value("value", 1));
        m_hidden = jItem.value("hidden", false);

        m_jAttributes = jBasic;
        jItem.erase("amount");
        for(auto it=jItem.begin(); it!=jItem.end(); ++it)
            m_jAttributes[it.key()] = it.value();
        m_jAttributes["id"] = m_sID;
    }

    // *** GETTER *** // 
    nlohmann::json getAttributes();
    std::string getType();
    std::string getAttack();
    std::string getFunction();
    size_t getEffekt();
    int getValue();
    bool getHidden();
    
    // *** SETTER *** //
    void setType(std::string sType);
    void setFunction(std::string sFunction);
    void setAttack(std::string sAttack);
    void setEffekt(size_t effekt);
    void setValue(int value);
    void setHidden(bool hidden); 

    static void initializeFunctions();
    bool callFunction(CPlayer* p);

    // *** FUNCTIONS *** //
    void equipeWeapon(CPlayer*);
    void consumeDrug(CPlayer*);
};

#endif

 
