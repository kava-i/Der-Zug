#ifndef CLISTENER_H
#define CLISTENER_H

#include <iostream>
#include <vector>
#include <regex>
#include <map>
#include <string>

#include "tools/func.h"

class CListener
{
  private:
    typedef std::pair<std::string, std::string> event;

  public:
    CListener(std::string id, std::string listener, std::string location, std::string location_id,
        std::string target_identifier, std::string new_identifier, int permeable, bool self_delete,
        int priority, std::string str_event_type);
    CListener(std::string id, std::string listener, std::string location, std::string location_id,
        std::string target_identifier, std::string new_identifier, int permeable, bool self_delete,
        int priority, std::regex regex_event_type, int pos);
    CListener(std::string id, std::string listener, std::string location, std::string location_id,
        std::string target_identifier, std::string new_identifier, int permeable, bool self_delete,
        int priority, std::vector<std::string> array_event_type);
    CListener(std::string id, std::string listener, std::string location, std::string location_id,
        std::string target_identifier, std::string new_identifier, int permeable, bool self_delete,
        int priority, std::map<std::string, std::string> map_event_type);

    static CListener* FromJson(nlohmann::json json, std::string location, std::string location_id);

    // getter 
    std::string id();
    std::string handler();  
    int permeable();  
    bool self_delete();
    int priority();
    std::string location();
    std::string location_id();

    /**
    * Function called from Context::throw_event, which will call the specific compare
    * function of this listener.
    * @return whether this listener pick up thrown on event.
    */
    bool check_match(event& newEvent);

  private:
    /// Id of listener. Also indicating which function to call.
    const std::string id_;
    const std::string handler_;  ///< handler called when listener triggers.
    const std::string location_;  ///< where listener comes from (room, char...); needed for deleting.
    const std::string location_id_;  ///< id of location (room_id, char_id, ...)
    
    // identifier and permeable
    const std::string target_identifier_;  ///< target_identifier f.e for pick up <item> specifies <item>
    const std::string new_identifier_;  ///< specifies follow up identifier.
    const int permeable_;  ///< 0/1 overwite contex/ handler permeability, -1 don't overwite
    const bool self_delete_;  ///< if set, listener deletes itself, after being called.
    const int priority_;

    // Event
    std::string event_type_str_;          ///< Used for direct string compare
    std::regex event_type_regex_;            ///< Match event-type with regex expression.
    int pos_;  ///< position of match in regex.
    std::vector<std::string> event_type_array_;      ///< Check if type exists in array.
    std::map<std::string, std::string> event_type_map_;  ///< Check if type exists in map.

    bool(CListener::*m_check_function)(event&);    ///< Custom compare function.

    // methods
    
    /**
    * Normal compare function. Case-sensitive and no partial matching!
    * param[in] sEventType (event-type thrown)
    * @return whether this listener picks up on thrown event.
    */
    bool StringCompare(event& newEvent);

    /**
    * Compares string with a regex-expression, allowing case insensitive and partial matching.
    * @param[in] sEventType (event-type thrown)
    * @return whether this listener picks up on thrown event.
    */
    bool RegexCompare(event& newEvent);

    /**
    * Checks whether thrown event exists in vector, or (if index) index is in
    * bounds of vector size.
    * @param[in] sEventType (event-type thrown)
    * @return whether this listener picks up on thrown event.
    */
    bool InVector(event& newEvent);

    /**
    * Check, whether a) searched string exists as a key, b) searched string compares fuzzy with
    * any value and c) compares with the position of an element.
    * @param[in] sEventType (event-type thrown)
    * @return whether this listener picks up on thrown event.
    */
    bool InMap(event& newEvent); 
};

#endif

