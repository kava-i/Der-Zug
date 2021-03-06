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
    std::string m_id;                                        
    
    std::string m_stringEventType;          ///< Used for direct string compare
    std::regex m_regexEventType;            ///< Match event-type with regex expression.
    std::vector<std::string> m_arrayEventType;      ///< Check if type exists in array.
    std::map<std::string, std::string> m_mapEventType;  ///< Check if type exists in map.

    int m_pos;

    typedef std::pair<std::string, std::string> event;
    bool(CListener::*m_check_function)(event&);    ///< Custom compare function.

public:

    CListener(std::string m_id, std::string sEventType);
    CListener(std::string m_id, std::regex sEventType, int pos);
    CListener(std::string m_id, std::vector<std::string> sEventType);
    CListener(std::string m_id, std::map<std::string, std::string> sEventType);

    /**
    * Return id, which also indicates, which function listener calls.
    * @return id
    */
    std::string getID();

    /**
    * Function called from Context::throw_event, which will call the specific compare
    * function of this listener.
    * @return whether this listener pick up thrown on event.
    */
    bool checkMatch(event& newEvent);

    /**
    * Normal compare function. Case-sensitive and no partial matching!
    * param[in] sEventType (event-type thrown)
    * @return whether this listener picks up on thrown event.
    */
    bool stringCompare(event& newEvent);

    /**
    * Compares string with a regex-expression, allowing case insensitive and partial matching.
    * @param[in] sEventType (event-type thrown)
    * @return whether this listener picks up on thrown event.
    */
    bool regexCompare(event& newEvent);

    /**
    * Checks whether thrown event exists in vector.
    * @param[in] sEventType (event-type thrown)
    * @return whether this listener picks up on thrown event.
    */
    bool inVector(event& newEvent);

    /**
    * Check, whether a) searched string exists as a key, b) searched string compares fuzzy with
    * any value and c) compares with the position of an element.
    * @param[in] sEventType (event-type thrown)
    * @return whether this listener picks up on thrown event.
    */
    bool inMap(event& newEvent); 
};

#endif

