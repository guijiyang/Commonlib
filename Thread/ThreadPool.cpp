#include "ThreadPool.h"
#include "Commonlib/MessageLoop/Task.h"
#include "Commonlib/Thread/Private/ThreadGroup.h"
#include "Commonlib/MessageLoop/Private/BlockingQueue.h"
#include "Commonlib/Thread/Private/TimerMessageLoop.h"

#include <assert.h>
#include <thread>
#include <unordered_map>

namespace Commonlib
{
    class CRegExecThreadPoolPrivate
    {
    public:
        CRegExecThreadPoolPrivate(int nThreadNum)
            :m_bExit(true),
            m_nMaxThreadNum(nThreadNum)
            {
            }

    public:
        CBlockingQueue<CTask*> m_taskQueue;
        CThreadGroup    m_threadGroup;
        bool            m_bExit;
        int             m_nMaxThreadNum;
    };

    CRegExecThreadPool::CRegExecThreadPool(int nThreadNum)
        :d_ptr(new CRegExecThreadPoolPrivate(nThreadNum))
    {
    }

    CRegExecThreadPool::~CRegExecThreadPool()
    {
        Stop();

        if(d_ptr != NULL)
        {
            delete d_ptr;
            d_ptr = NULL;
        }
    }

    bool CRegExecThreadPool::Start()
    {
        CL_D(CRegExecThreadPool);
        if(d->m_nMaxThreadNum <= 0 || d->m_threadGroup.size() > 0)
        {
            return false;
        }

        d->m_bExit = false;
        for(int i = 0; i < d->m_nMaxThreadNum; ++i)
        {
            d->m_threadGroup.AddThread(new std::thread(std::bind(&CRegExecThreadPool::Run, this)));
        }

        return true;
    }

    void CRegExecThreadPool::Stop()
    {
        CL_D(CRegExecThreadPool);
        d->m_bExit = true;
        d->m_taskQueue.NotifyAll();
        d->m_threadGroup.JoinAll();
    }

    bool CRegExecThreadPool::IsRunning() const
    {
        CL_D(const CRegExecThreadPool);
        return !d->m_bExit;
    }

    void CRegExecThreadPool::Run()
    {
        CL_D(CRegExecThreadPool);
        for(;!d->m_bExit;)
        {
            CTask* pTask = d->m_taskQueue.TakeFromQueue();
            if(NULL != pTask)
            {
                pTask->Exec();
                delete pTask;
                pTask = NULL;
            }
        }
    }

    bool CRegExecThreadPool::PostTask(CTask* pTask)
    {
        CL_D(CRegExecThreadPool);
        if(d->m_bExit)
        {
            if(NULL != pTask)
            {
                delete pTask;
                pTask = NULL;
            }

            return false;
        }

        d->m_taskQueue.AddToQueue(pTask);

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    typedef std::unordered_map<std::string, CTimerMessageLoop*> mapMsgLoop;
    typedef mapMsgLoop::iterator mapMsgLoopIter;
    typedef mapMsgLoop::const_iterator mapConstMsgLoopIter;

    class CCachedThreadPoolPrivate
    {
    public:
        CCachedThreadPoolPrivate()
            : m_bExit(true)
        {
        }

        CTimerMessageLoop* FindMsgLoop(const std::string& strID) const
        {
            CRMutexLock lock(m_mutex, std::defer_lock);
            mapConstMsgLoopIter iter = m_mapMsgLoop.find(strID);
            if(iter != m_mapMsgLoop.end())
            {
                return iter->second;
            }

            return NULL;
        }

    public:
        mutable CWRMutex m_mutex;
        mapMsgLoop m_mapMsgLoop;
        bool m_bExit;
    };

    CCachedThreadPool::CCachedThreadPool()
        : d_ptr(new CCachedThreadPoolPrivate())
    {
    }

    CCachedThreadPool::~CCachedThreadPool()
    {
        Stop();
        if(NULL != d_ptr)
        {
            delete d_ptr;
            d_ptr = NULL;
        }
    }

    bool CCachedThreadPool::Start()
    {
        CL_D(CCachedThreadPool);

        CWMutexLock lock(d->m_mutex);
        if(!d->m_bExit)
        {
            return true;
        }

        d->m_bExit = false;
        return true;
    }

    void CCachedThreadPool::Stop()
    {
        CL_D(CCachedThreadPool);

        CWMutexLock lock(d->m_mutex);
        d->m_bExit = true;
        for(auto iter = d->m_mapMsgLoop.begin(); iter != d->m_mapMsgLoop.end();)
        {
            CTimerMessageLoop* pMsgLoop = iter->second;
            if(NULL == pMsgLoop)
            {
                assert(NULL);
                continue;
            }

            iter = d->m_mapMsgLoop.erase(iter);
            pMsgLoop->Stop();
            delete pMsgLoop;
        }

        d->m_mapMsgLoop.clear();
    }

    bool CCachedThreadPool::PostTask(const std::string& strID, CTask* pTask)
    {
        CL_D(CCachedThreadPool);

        {
            CRMutexLock lock(d->m_mutex, std::defer_lock);
            if(d->m_bExit)
            {
                if(NULL != pTask)
                {
                    delete pTask;
                    pTask = NULL;
                }
                return false;
            }
        }

        {
            //已存在此队列
            CRMutexLock lock(d->m_mutex, std::defer_lock);
            auto iter = d->m_mapMsgLoop.find(strID);
            if(iter != d->m_mapMsgLoop.end())
            {
                CTimerMessageLoop* pMsgLoop = iter->second;
                if(NULL == pMsgLoop)
                {
                    if(NULL != pTask)
                    {
                        delete pTask;
                        pTask = NULL;
                    }

                    assert(NULL);
                    return false;
                }

                return pMsgLoop->PostTask(pTask);
            }
        }

        //不存在此队列
        CWMutexLock lock(d->m_mutex);
        for(auto iter = d->m_mapMsgLoop.begin(); iter != d->m_mapMsgLoop.end(); ++iter)
        {
            CTimerMessageLoop* pMsgLoop = iter->second;
            if(NULL == pMsgLoop)
            {
                if(NULL != pTask)
                {
                    delete pTask;
                    pTask = NULL;
                }

                assert(NULL);
                return false;
            }

            //存在空闲线程队列，将任务塞入该队列，并重置队列标识码
            if(pMsgLoop->IsIdeal())
            {
                d->m_mapMsgLoop.erase(iter);
                d->m_mapMsgLoop.insert(make_pair(strID, pMsgLoop));
                return pMsgLoop->PostTask(pTask);
            }
        }

        //不存在空闲，创建队列
        CTimerMessageLoop* pMsgLoop = new CTimerMessageLoop();
        pMsgLoop->Start();
        d->m_mapMsgLoop.insert(make_pair(strID, pMsgLoop));
        return pMsgLoop->PostTask(pTask);
    }

    void CCachedThreadPool::ClearTask(const std::string& strID)
    {
        CL_D(CCachedThreadPool);

        CTimerMessageLoop* pMsgLoop = d->FindMsgLoop(strID);
        if(NULL == pMsgLoop)
        {
            return;
        }

        pMsgLoop->ClearTask();
    }

    void CCachedThreadPool::AsyncClearTask(const std::string& strID)
    {
        CL_D(CCachedThreadPool);

        CTimerMessageLoop* pMsgLoop = d->FindMsgLoop(strID);
        if(NULL == pMsgLoop)
        {
            return;
        }

        pMsgLoop->AsyncClearTask();
    }
}