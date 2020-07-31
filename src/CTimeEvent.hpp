#ifndef CTIMEEVENT_H
#define CTIMEEVENT_H

#include <iostream>
#include <vector>
#include <regex>
#include <map>
#include <string>
#include <time.h>
#include <ctime>
#include <chrono>
#include "func.hpp"

class CTimeEvent
{
private:
    //std::string m_id;
    std::string m_type;
    std::string m_scope;
    std::string m_info;
    std::string m_function;
    typedef std::chrono::system_clock::time_point clock_start;
    clock_start m_start;
    double m_duration;
    
public:
    
    //Constructor
    CTimeEvent(std::string type, std::string scope, std::string func, std::string info, double duration)
    {
        m_type = type;
        m_scope = scope;
        m_function = func;
        m_info = info;
        m_start = std::chrono::system_clock::now();
        m_duration = duration*60;
    }

    // *** getter *** //
    
    ///Get type
    std::string getType()       { return m_type; }
    ///Get scope.
    std::string getScope()      { return m_scope; }
    ///Get info.
    std::string getInfo()       { return m_info; }
    ///Get function.
    std::string getFunction()   { return m_function; }
    //Get start.
    clock_start getStart()      { return m_start; }
    ///Get duration.
    double getDuration()        { return m_duration; }
    
}; 

#endif
