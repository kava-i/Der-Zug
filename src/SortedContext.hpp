#pragma once
#include <map>
#include <queue>
#include <list>
#include "CContext.hpp"
#include "Catch2/single_include/catch2/catch.hpp"

class CContextStack
{
    private:
	std::map<std::string,CContext*> m_contextStack;
	std::vector<std::pair<CContext*,int>> m_sortedContexts;
	std::deque<CContext*> m_sortedQueue;
	bool m_reloadQueue;

    public:
	CContextStack();
	void insert(CContext* ctx, int priority, std::string name);
	void erase(std::string name);
	const std::deque<CContext*> &getSortedCtxList();
	bool nonPermeableContextInList();
};
