#pragma once
#include <map>
#include <queue>
#include <list>
#include "CEnhancedContext.hpp"
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
	CContextStack();
	void insert(T* ctx, int priority, std::string name);
	void erase(std::string name);
    T* getContext(std::string sName)
    {
        if(m_contextStack.count(sName) >= 0)
            return m_contextStack[sName];
        else
            return NULL;
    }

	const std::deque<T*> &getSortedCtxList();
	bool nonPermeableContextInList();
};
