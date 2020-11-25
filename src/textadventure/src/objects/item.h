#ifndef SRC_OBJECTS_ITEM_H
#define SRC_OBJECTS_ITEM_H

#include <iostream>
#include <map>

#include <nlohmann/json.hpp>

#include "object.h"
#include "tools/func.h"

class CPlayer;

class CItem : public CObject
{
protected:

  nlohmann::json m_jAttributes;
  CText* m_useDescription;             ///< Description when item is used.
  std::string m_sCategory;        
  std::string m_sType;
  std::string m_sFunction;
  std::string m_sAttack;
  size_t m_effekt;
  int m_value;
  bool m_hidden;

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
  std::string getType();
  std::string getAttack();
  std::string getFunction();
  size_t getEffekt();
  int getValue();
  bool getHidden();

  //Book
  size_t getMark();
  CText* getPages();
  
  // *** SETTER *** //
  void setCategory(std::string sCategory);
  void setType(std::string sType);
  void setFunction(std::string sFunction);
  void setAttack(std::string sAttack);
  void setEffekt(size_t effekt);
  void setValue(int value);
  void setHidden(bool hidden); 
  void setMark(size_t mark);

  std::string getAllInfos(); 
  
  static void initializeFunctions();
  bool callFunction(CPlayer* p);

  // *** FUNCTIONS *** //
  void equipe(CPlayer*);
  void consume(CPlayer*);
  void read(CPlayer*);
};

#endif
