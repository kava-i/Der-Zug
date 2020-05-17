#include "CListener.hpp"

// ***** CONSTRUCTOR ***** //
CListener::CListener(std::string id, std::string sEventType)
{
    m_id = id;
    m_stringEventType = sEventType;
    m_check_function = &CListener::stringCompare;
}
    
CListener::CListener(std::string id, std::regex eventType, int pos)
{
    m_id = id;
    m_regexEventType = eventType;
    m_pos = pos;
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
bool CListener::checkMatch(event& e) {
    return (this->*m_check_function)(e);
}

/**
* Normal compare function. Case-sensitive and no partial matching!
* param[in] sEventType (event-type thrown)
* @return whether this listener picks up on thrown event.
*/
bool CListener::stringCompare(event& e) {
    return e.first == m_stringEventType;
}

/**
* Compares string with a regex, allowing case insensitive and partial matching.
* @param[in] sEventType (event-type thrown)
* @return whether this listener picks up on thrown event.
*/
bool CListener::regexCompare(event& e) {
    std::string str = e.first + " " + e.second;
    std::smatch m;
    if(std::regex_match(str, m, m_regexEventType))
    {
        e.second = m[m_pos];
        return true;
    }
    return false;
}

/**
* Checks whether thrown event exists in vector.
* @param[in] sEventType (event-type thrown)
* @return whether this listener picks up on thrown event.
*/
bool CListener::inVector(event& e) {
    if(std::isdigit(e.first[0]) && stoul(e.first, nullptr, 0) <= m_arrayEventType.size() && stoul(e.first, nullptr, 0) > 0)
        return true;
    return func::inArray(m_arrayEventType, e.first);
}

/**
* Check, whether a) searched string exists as a key, b) searched string compares fuzzy with
* any value and c) compares with the position of an element.
* @param[in] sEventType (event-type thrown)
* @return whether this listener picks up on thrown event.
*/
bool CListener::inMap(event& e) {
    return func::getObjectId(m_mapEventType, e.first) != "";
}
