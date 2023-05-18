#pragma once
#include <iostream>
#include <map>
#include <queue>
#include <list>

template<class T>
class CContextStack
{
private:
	std::map<std::string,T*> m_contextStack;
	std::vector<std::pair<T*,int>> m_sortedContexts;
	std::deque<T*> m_sortedQueue;
	bool m_reloadQueue;

public:
	CContextStack() {
    m_reloadQueue = false;
  }

  /**
  * insert new context into map with given priority.
  * @param[in] ctx (context to add)
  * @param[in] priority (stack context above or underneath other contexts)
  * @param[in] name (name of context)
  */ 
  void insert(T* ctx, int priority, std::string name) {
    m_contextStack[name] = ctx;
    m_sortedContexts.push_back(std::pair<T*, int>{ctx, priority});
    std::sort(m_sortedContexts.begin(), m_sortedContexts.end(),
        [](std::pair<T*,int>& a,std::pair<T*,int>& b)
        { return a.second<b.second; });
    m_reloadQueue = true;
  }

  /**
  * erase context from stack.
  * @param[in] name (name of context)
  */
  void erase(std::string name) {
    T* ctx = nullptr;
    try {
      ctx = m_contextStack.at(name);
      m_reloadQueue = true;
      m_contextStack.erase(name);
      m_sortedContexts.erase(std::remove_if(m_sortedContexts.begin(),
          m_sortedContexts.end(),[ctx](const std::pair<T*,int> &p) { 
            if (p.first == ctx) return true; 
            else return false;
          }), m_sortedContexts.end());
    }
    catch(...) {
      return;
    }
  }

  void eraseByPointer(T* ctx) {
    m_reloadQueue = true;
    // Comare pointers
    auto lambda = [&](std::pair<T*, int>& a) {return a.first == ctx;};
    auto it = std::find_if(m_sortedContexts.begin(), m_sortedContexts.end(), lambda);
    if (it != m_sortedContexts.end()) {
      
      // Store id, delete context and remove from sorted-list.
      std::string name = ctx->getID();
      delete it->first;
      m_sortedContexts.erase(it);
      std::cout << "Deleted listener: " << name << std::endl;

      // Check if there is another context with the same name.
      auto lambda = [&](std::pair<T*, int>& a) {return a.first->getID() == name;};
      auto it = std::find_if(m_sortedContexts.begin(), m_sortedContexts.end(), lambda);

      // If yes erase. 
      if (it == m_sortedContexts.end())
        m_contextStack.erase(name);
      // Otherwise, if the current map entry is null set above entry: 
      else if (m_contextStack.count(name) > 0 && m_contextStack[name]) {
        m_contextStack[name] = it->first;
      }
    }
  }

  /**
  * Empty complete stack.
  */
  void clear() {
    m_contextStack.clear();
    m_sortedContexts.clear();
    m_sortedQueue.clear();
  }

  /**
  * Return a context. Achieve context by name.
  * @param[in] sName (name of context to return)
  * @return return given context.
  */
  T* getContext(std::string sName) {
    if (m_contextStack.count(sName) >= 0)
      return m_contextStack[sName];
    else
      return NULL;
  }

  /**
  * @return a list of contexts in sorted order.
  */ 
  const std::deque<T*> &getSortedCtxList() {
    if (m_reloadQueue) {
      m_sortedQueue.clear();
      for (auto &it : m_sortedContexts)
        m_sortedQueue.push_front(it.first);
      m_reloadQueue = false;
    }
    return m_sortedQueue;
  }

  /**
  * @return whether context-stack contains a non-permeable context.
  */
  bool nonPermeableContextInList() {
    return std::accumulate(m_sortedContexts.begin(),
        m_sortedContexts.end(), 0, [](int b,std::pair<T*,int> &k)
        { if(k.first->getPermeable()) return b; else return b+1;}) > 1;
  }
};
