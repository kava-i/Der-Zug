#include "sorted_context.h"


/*
TEST_CASE("Testing permeable context stacks","[CContextStack]")
{
    CContextStack<Context> st;
    Context cdiag((std::string)"fight");
    Context cworld((std::string)"world");
    st.insert((Context*)&cdiag,10,"diag");
    st.insert((Context*)&cworld,11,"world");
    REQUIRE(st.nonPermeableContextInList()==false);
    st.erase("diag");
    REQUIRE(st.getSortedCtxList().size() == 1);
    REQUIRE(st.nonPermeableContextInList()==false);
}

TEST_CASE("Testing CContextStack","[CContextStack]")
{
    CContextStack<Context> st;
    st.insert((Context*)0,0,"alex");
    st.insert((Context*)1,1,"blex");
    st.insert((Context*)2,2,"clex");
    st.insert((Context*)3,3,"dlex");
    REQUIRE( st.getSortedCtxList().size() == 4);
    st.erase("dlex");
    REQUIRE( st.getSortedCtxList().size() == 3);
    REQUIRE( st.getSortedCtxList().front() == (Context*)2);
    st.erase("ogo");
    REQUIRE( st.getSortedCtxList().size() == 3);
    REQUIRE( st.getSortedCtxList().front() == (Context*)2);
    st.insert((Context*)4,10,"flex");
    REQUIRE( st.getSortedCtxList().front() == (Context*)4);
}
*/
