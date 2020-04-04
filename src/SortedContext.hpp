#pragma once
#include <map>
#include <queue>
#include <list>
#include "CEnhancedContext.hpp"
#include <catch2/catch.hpp>

class CContextStack
{
    private:
	std::map<std::string,CEnhancedContext*> m_contextStack;
	std::vector<std::pair<CEnhancedContext*,int>> m_sortedContexts;
	std::deque<CEnhancedContext*> m_sortedQueue;
	bool m_reloadQueue;

    public:
	CContextStack();
	void insert(CEnhancedContext* ctx, int priority, std::string name);
	void erase(std::string name);
    CEnhancedContext* getContext(std::string sName);
	const std::deque<CEnhancedContext*> &getSortedCtxList();
	bool nonPermeableContextInList();
};
