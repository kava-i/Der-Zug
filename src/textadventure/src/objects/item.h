#ifndef SRC_OBJECTS_ITEM_H
#define SRC_OBJECTS_ITEM_H

#include <iostream>
#include <map>

#include <nlohmann/json.hpp>

#include "object.h"
#include "tools/func.h"

class CPlayer;

class CItem : public CObject {
  protected:
    nlohmann::json json_;
    CText* use_description_;             ///< Description when item is used.
    std::string category_;        
    std::string kind_;
    std::string type_;
    std::string attack_;
    bool hidden_;

    Updates updates_;
    Updates costs_;

    //Book
    size_t m_mark;
    CText* m_pages;

    ///Static map f all state-functions
    static std::map<std::string, void (CItem::*)(CPlayer*)> m_functions;

  public:
    /**
    * Constructor for non-derived items simply placing the passed json as items attributes.
    * @param[in] basic attributes from base item.
    */
    CItem(nlohmann::json jBasic, CPlayer* p);

    // *** GETTER *** // 
    nlohmann::json getAttributes();
    std::string getCategory();
    std::string kind();
    std::string type();
    std::string getAttack();
    bool getHidden();
    const Updates& update();
    const Updates& costs();
    CText* use_description();

    // Book
    size_t getMark();
    CText* getPages();
    
    // *** SETTER *** //
    void setCategory(std::string sCategory);
    void setKind(std::string kind);
    void setAttack(std::string sAttack);
    void setHidden(bool hidden); 
    void setMark(size_t mark);

    std::string getAllInfos(); 
    std::string Costs();
    
    static void initializeFunctions();
    /**
     * Calls category-function or prints useDescription. 
     * - `consume`: prints use-desc, applies updates, removes item
     * - `equipe`: call player equipe function 
     * - 
     */
    bool callFunction(CPlayer* p);

    // *** FUNCTIONS *** //
    void equipe(CPlayer*);
    void consume(CPlayer*);
    void read(CPlayer*);

    // helper 
    void PrintUseAndDoUpdate(std::string default_use, CPlayer* p);
};

#endif
