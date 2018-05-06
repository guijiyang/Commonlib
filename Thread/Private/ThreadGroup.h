#ifndef COMMONLIB_THREADGROUP_H
#define COMMONLIB_THREADGROUP_H

#include "Commonlib/Thread/Mutex.h"

#include <thread>
#include <list>
#include <memory>
#include <algorithm>

namespace Commonlib
{
    class CThreadGroup
    {
    public:
        CThreadGroup(){}
        ~CThreadGroup()
        {
            for(auto iter = m_lstThreads.begin(); iter != m_lstThreads.end(); ++iter)
            {
                delete *iter;
            }
        }

    public:
        template<typename Func>
        std::thread* CreateThread(Func threadFunc)
        {
            CMutexLock lock(m_mutex);
            std::unique_ptr<std::thread> pThread(new std::thread(threadFunc));
            m_lstThreads.push_back(pThread.get());
            return pThread.release();
        }

        void AddThread(std::thread* thrd)
        {
            if(thrd)
            {
                CMutexLock lock(m_mutex);
                m_lstThreads.push_back(thrd);
            }
        }

        void RemoveThread(std::thread* thrd)
        {
            CMutexLock lock(m_mutex);
            if(thrd)
            {
                auto iter = std::find(m_lstThreads.begin(), m_lstThreads.end(), thrd);
                if(iter != m_lstThreads.end())
                {
                    m_lstThreads.erase(iter);
                }
            }
        }

        void JoinAll()
        {
            CMutexLock lock(m_mutex);
            for(auto iter = m_lstThreads.begin(); iter != m_lstThreads.end(); ++iter)
            {
                (*iter)->join();
            }
        }

        size_t size() const
        {
            return m_lstThreads.size();
        }
    private:
        std::list<std::thread*> m_lstThreads;
        mutable CMutex m_mutex;
    };
}

#endif //COMMONLIB_THREADGROUP_H