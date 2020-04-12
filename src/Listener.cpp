#include "CListener.hpp"

// ***** CONSTRUCTOR ***** //
CListener::CListener(std::string id, std::string sEventType)
{
    m_id = id;
    m_stringEventType = sEventType;
    m_check_function = &CListener::stringCompare;
}
    
CListener::CListener(std::string id, std::regex eventType)
{
    m_id = id;
    m_regexEventType = eventType;
    m_check_function = &CListener::regexCompare;
}

CListener::CListener(std::string id, std::vector<std::string> eventType)
{
    m_id = id;
    m_arrayEventType = eventType;
    m_check_function = &CListener::inVector;
}

CListener::CListener(std::string id, std::map<std::string, std::string> eventType)
{
    m_id = id;
    m_mapEventType = eventType;
    m_check_function = &CListener::inMap;
}

// *** GETTER *** //

/**
* Return id, which also indicates, which function listener calls.
* @return id
*/
std::string CListener::getID() {
    return m_id;
}


// ***** FUNCTIONS ***** // 
/**

* Function called from Context::throw_event, which will call the specific compare
* function of this listener.
* @param[in] sEventType (event-type thrown)
* @return whether this listener pick up thrown on event.
*/
bool CListener::checkMatch(std::string sEventType) {
    return (this->*m_check_function)(sEventType);
}

/**
* Normal compare function. Case-sensitive and no partial matching!
* param[in] sEventType (event-type thrown)
* @return whether this listener picks up on thrown event.
*/
bool CListener::stringCompare(std::string sEventType) {
    return sEventType == m_stringEventType;
}

/**
* Compares string with a regex, allowing case insensitive and partial matching.
* @param[in] sEventType (event-type thrown)
* @return whether this listener picks up on thrown event.
*/
bool CListener::regexCompare(std::string sEventType) {
    return std::regex_match(sEventType, m_regexEventType);    
}

/**
* Checks whether thrown event exists in vector.
* @param[in] sEventType (event-type thrown)
* @return whether this listener picks up on thrown event.
*/
bool CListener::inVector(std::string sEventType) {
    if(std::isdigit(sEventType[0]) && stoul(sEventType, nullptr, 0) <= m_arrayEventType.size() && stoul(sEventType, nullptr, 0) > 0)
        return true;
    return func::inArray(m_arrayEventType, sEventType);
}

/**
* Check, whether a) searched string exists as a key, b) searched string compares fuzzy with
* any value and c) compares with the position of an element.
* @param[in] sEventType (event-type thrown)
* @return whether this listener picks up on thrown event.
*/
bool CListener::inMap(std::string sEventType) {
    return func::getObjectId(m_mapEventType, sEventType) != "";
}
