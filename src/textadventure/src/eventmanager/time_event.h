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

#include "tools/func.h"

class CTimeEvent
{
private:
    std::string m_id;
    std::string m_scope;
    std::string m_info;
    typedef std::chrono::system_clock::time_point clock_start;
    clock_start m_start;
    double m_duration;
    
public:
    
    //Constructor
    CTimeEvent(std::string id, std::string scope, std::string info, double duration)
    {
        m_id = id;
        m_scope = scope;
        m_info = info;
        m_start = std::chrono::system_clock::now();
        m_duration = duration*60;
    }

    // *** getter *** //
    
    ///Get type
    std::string getID()         { return m_id; }
    ///Get scope.
    std::string getScope()      { return m_scope; }
    ///Get info.
    std::string getInfo()       { return m_info; }
    //Get start.
    clock_start getStart()      { return m_start; }
    ///Get duration.
    double getDuration()        { return m_duration; }
    
}; 

#endif
