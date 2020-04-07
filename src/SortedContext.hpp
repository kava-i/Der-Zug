#pragma once
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
