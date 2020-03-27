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
    std::string m_sCategory;
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
    CItem(nlohmann::json jBasic, CPlayer* p);

    /**
    * Constructor for derived items. The basic json usually supplies basic attributes. Which 
    * might get overridden, by specific attributes, which otherwise only supply extra item
    * specific attributes.
    * @param[in] jBasic basic attributes from base item.
    * @param[in] jItem extra attributes from specific individual item.
    */
    CItem(nlohmann::json jBasic, nlohmann::json jItem, CPlayer* p, std::string sID="");

    // *** GETTER *** // 
    nlohmann::json getAttributes();
    std::string getCategory();
    std::string getType();
    std::string getAttack();
    std::string getFunction();
    size_t getEffekt();
    int getValue();
    bool getHidden();
    
    // *** SETTER *** //
    void setCategory(std::string sCategory);
    void setType(std::string sType);
    void setFunction(std::string sFunction);
    void setAttack(std::string sAttack);
    void setEffekt(size_t effekt);
    void setValue(int value);
    void setHidden(bool hidden); 

    static void initializeFunctions();
    bool callFunction(CPlayer* p);

    // *** FUNCTIONS *** //
    void equipe(CPlayer*);
    void consume(CPlayer*);
};

#endif

 
