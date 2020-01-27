#include "SortedContext.hpp"

CContextStack::CContextStack()
{
    m_reloadQueue = false;
}

void CContextStack::insert(CContext *ctx, int priority, std::string name)
{
    m_contextStack[name] = ctx;
    m_sortedContexts.push_back(std::pair<CContext*,int>{ctx,priority});
    std::sort(m_sortedContexts.begin(),m_sortedContexts.end(),[](std::pair<CContext*,int>& a,std::pair<CContext*,int>& b){return a.second<b.second;});
    m_reloadQueue = true;
}

void CContextStack::erase(std::string name)
{
    CContext* ctx = nullptr;
    try
    {
	ctx = m_contextStack.at(name);
	m_reloadQueue = true;
	m_contextStack.erase(name);
	m_sortedContexts.erase(std::remove_if(m_sortedContexts.begin(),m_sortedContexts.end(),[ctx](const std::pair<CContext*,int> &p){if(p.first==ctx)return true; else return false;}),m_sortedContexts.end());
    }
    catch(...)
    {
	return;
    }
}

const std::deque<CContext*> &CContextStack::getSortedCtxList()
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
    return std::accumulate(m_sortedContexts.begin(),m_sortedContexts.end(),false,[](bool b,std::pair<CContext*,int> &k){return b|!k.first->getPermeable();});
}

#include "CWorldContext.hpp"
#include "CDialogContext.hpp"
TEST_CASE("Testing permeable context stacks","[CContextStack]")
{
    CContextStack st;
    CDialogContext cdiag;
    CWorldContext cworld;
    st.insert((CContext*)&cdiag,10,"diag");
    st.insert((CContext*)&cworld,11,"world");
    REQUIRE(st.nonPermeableContextInList());
    st.erase("diag");
    REQUIRE(st.getSortedCtxList().size() == 1);
    REQUIRE(st.nonPermeableContextInList()==false);
}

TEST_CASE("Testing CContextStack","[CContextStack]")
{
    CContextStack st;
    st.insert((CContext*)0,0,"alex");
    st.insert((CContext*)1,1,"blex");
    st.insert((CContext*)2,2,"clex");
    st.insert((CContext*)3,3,"dlex");
    REQUIRE( st.getSortedCtxList().size() == 4);
    st.erase("dlex");
    REQUIRE( st.getSortedCtxList().size() == 3);
    REQUIRE( st.getSortedCtxList().front() == (CContext*)2);
    st.erase("ogo");
    REQUIRE( st.getSortedCtxList().size() == 3);
    REQUIRE( st.getSortedCtxList().front() == (CContext*)2);
    st.insert((CContext*)4,10,"flex");
    REQUIRE( st.getSortedCtxList().front() == (CContext*)4);
}
