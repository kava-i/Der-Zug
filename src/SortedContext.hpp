#pragma once
#include <iostream>
#include <map>
#include <queue>
#include <list>
#include <catch2/catch.hpp>

template<class T>
class CContextStack
{
private:
	std::map<std::string,T*> m_contextStack;
	std::vector<std::pair<T*,int>> m_sortedContexts;
	std::deque<T*> m_sortedQueue;
	bool m_reloadQueue;

public:
	CContextStack()
    {
        m_reloadQueue = false;
    }


    /**
    * insert new context into map with given priority.
    * @param[in] ctx (context to add)
    * @param[in] priority (stack context above or underneath other contexts)
    * @param[in] name (name of context)
    */ 
	void insert(T* ctx, int priority, std::string name)
    {
        m_contextStack[name] = ctx;
        m_sortedContexts.push_back(std::pair<T*,int>{ctx,priority});
        std::sort(m_sortedContexts.begin(),m_sortedContexts.end(),[](std::pair<T*,int>& a,std::pair<T*,int>& b){return a.second<b.second;});
        m_reloadQueue = true;
    }

    /**
    * erase context from stack.
    * @param[in] name (name of context)
    */
	void erase(std::string name)
    {
        T* ctx = nullptr;
        try
        {
            ctx = m_contextStack.at(name);
            m_reloadQueue = true;
            m_contextStack.erase(name);
            m_sortedContexts.erase(std::remove_if(m_sortedContexts.begin(),m_sortedContexts.end(),[ctx](const std::pair<T*,int> &p){if(p.first==ctx)return true; else return false;}),m_sortedContexts.end());
        }
        catch(...)
        {
            return;
        }
    }

    /**
    * erase highest priority from stack.
    * @param[in] name
    */
    void eraseHighest(std::string name)
    {
        if(m_contextStack.count(name) == 0)
            return;
 
        //Make sure still sorted and set queue to relod
         m_reloadQueue = true;
        std::sort(m_sortedContexts.begin(),m_sortedContexts.end(),[](std::pair<T*,int>& a,std::pair<T*,int>& b){return a.second<b.second;});
        std::reverse(m_sortedContexts.begin(), m_sortedContexts.end());

        auto lambda = [&](std::pair<T*, int>& a) { return a.first->getID() == name; };

        //Delete first occurance
        auto it = std::find_if(m_sortedContexts.begin(), m_sortedContexts.end(), lambda);
        if(it != m_sortedContexts.end())
            m_sortedContexts.erase(it);

        //Delete in map if empty
        if(std::find_if(m_sortedContexts.begin(), m_sortedContexts.end(), lambda) == m_sortedContexts.end())
            m_contextStack.erase(name);
    }

    /**
    * Empty complete stack.
    */
    void clear()
    {
        m_contextStack.clear();
        m_sortedContexts.clear();
        m_sortedQueue.clear();
    }

    /**
    * Return a context. Achieve context by name.
    * @param[in] sName (name of context to return)
    * @return return given context.
    */
    T* getContext(std::string sName)
    {
        if(m_contextStack.count(sName) >= 0)
            return m_contextStack[sName];
        else
            return NULL;
    }

    /**
    * @return a list of contexts in sorted order.
    */ 
	const std::deque<T*> &getSortedCtxList()
    {
        if(m_reloadQueue)
        {
            m_sortedQueue.clear();
            for(auto &it : m_sortedContexts)
                m_sortedQueue.push_front(it.first);
            m_reloadQueue = false;
        }
        return m_sortedQueue;
    }

    /**
    * @return whether context-stack contains a non-permeable context.
    */
	bool nonPermeableContextInList()
    {
        return std::accumulate(m_sortedContexts.begin(),m_sortedContexts.end(),0,[](int b,std::pair<T*,int> &k){if(k.first->getPermeable())return b;else return b+1;}) > 1;
    }
};
