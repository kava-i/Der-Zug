#include "SortedContext.hpp"


/*
TEST_CASE("Testing permeable context stacks","[CContextStack]")
{
    CContextStack<CEnhancedContext> st;
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
    CContextStack<CEnhancedContext> st;
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
*/
