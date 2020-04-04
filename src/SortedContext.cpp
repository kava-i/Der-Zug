#include "SortedContext.hpp"

CContextStack::CContextStack()
{
    m_reloadQueue = false;
}

void CContextStack::insert(CEnhancedContext*ctx, int priority, std::string name)
{
    m_contextStack[name] = ctx;
    ctx->setName(name);
    m_sortedContexts.push_back(std::pair<CEnhancedContext*,int>{ctx,priority});
    std::sort(m_sortedContexts.begin(),m_sortedContexts.end(),[](std::pair<CEnhancedContext*,int>& a,std::pair<CEnhancedContext*,int>& b){return a.second<b.second;});
    m_reloadQueue = true;
}

void CContextStack::erase(std::string name)
{
    CEnhancedContext* ctx = nullptr;
    try
    {
	ctx = m_contextStack.at(name);
	m_reloadQueue = true;
	m_contextStack.erase(name);
	m_sortedContexts.erase(std::remove_if(m_sortedContexts.begin(),m_sortedContexts.end(),[ctx](const std::pair<CEnhancedContext*,int> &p){if(p.first==ctx)return true; else return false;}),m_sortedContexts.end());
    }
    catch(...)
    {
	return;
    }
}

CEnhancedContext* CContextStack::getContext(std::string sName)
{
    if(m_contextStack.count(sName) >= 0)
        return m_contextStack[sName];
    else
        return NULL;
}

const std::deque<CEnhancedContext*> &CContextStack::getSortedCtxList()
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

bool CContextStack::nonPermeableContextInList()
{
    return std::accumulate(m_sortedContexts.begin(),m_sortedContexts.end(),0,[](int b,std::pair<CEnhancedContext*,int> &k){if(k.first->getPermeable())return b;else return b+1;}) > 1;
}

TEST_CASE("Testing permeable context stacks","[CContextStack]")
{
    CContextStack st;
    CEnhancedContext cdiag((std::string)"fight");
    CEnhancedContext cworld((std::string)"world");
    st.insert((CEnhancedContext*)&cdiag,10,"diag");
    st.insert((CEnhancedContext*)&cworld,11,"world");
    REQUIRE(st.nonPermeableContextInList()==false);
    st.erase("diag");
    REQUIRE(st.getSortedCtxList().size() == 1);
    REQUIRE(st.nonPermeableContextInList()==false);
}

TEST_CASE("Testing CContextStack","[CContextStack]")
{
    CContextStack st;
    st.insert((CEnhancedContext*)0,0,"alex");
    st.insert((CEnhancedContext*)1,1,"blex");
    st.insert((CEnhancedContext*)2,2,"clex");
    st.insert((CEnhancedContext*)3,3,"dlex");
    REQUIRE( st.getSortedCtxList().size() == 4);
    st.erase("dlex");
    REQUIRE( st.getSortedCtxList().size() == 3);
    REQUIRE( st.getSortedCtxList().front() == (CEnhancedContext*)2);
    st.erase("ogo");
    REQUIRE( st.getSortedCtxList().size() == 3);
    REQUIRE( st.getSortedCtxList().front() == (CEnhancedContext*)2);
    st.insert((CEnhancedContext*)4,10,"flex");
    REQUIRE( st.getSortedCtxList().front() == (CEnhancedContext*)4);
}
