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

    /// Id of listener. Also indicating which function to call.
    std::string id_;                                        
    
    std::string event_type_str_;          ///< Used for direct string compare
    std::regex event_type_regex_;            ///< Match event-type with regex expression.
    std::vector<std::string> event_type_array_;      ///< Check if type exists in array.
    std::map<std::string, std::string> event_type_map_;  ///< Check if type exists in map.

    int m_pos;

    typedef std::pair<std::string, std::string> event;
    bool(CListener::*m_check_function)(event&);    ///< Custom compare function.

public:

    CListener(std::string id, std::string event_type);
    CListener(std::string id, std::regex event_type, int pos);
    CListener(std::string id, std::vector<std::string> event_type);
    CListener(std::string id, std::map<std::string, std::string> event_type);

    /**
    * Return id, which also indicates, which function listener calls.
    * @return id
    */
    std::string id();

    /**
    * Function called from Context::throw_event, which will call the specific compare
    * function of this listener.
    * @return whether this listener pick up thrown on event.
    */
    bool check_match(event& newEvent);

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

