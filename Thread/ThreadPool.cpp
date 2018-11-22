#include "ThreadPool.h"
#include "MessageLoop/Task.h"
#include "Thread/Private/ThreadGroup.h"
#include "MessageLoop/Private/BlockingQueue.h"
#include "Thread/Private/TimerMessageLoop.h"

#include <assert.h>
#include <thread>
#include <unordered_map>

namespace Commonlib
{
class CRegExecThreadPoolPrivate
{
  public:
    CRegExecThreadPoolPrivate(int nThreadNum, int nMaxThreadNum)
        : m_bExit(true),
          m_nThreadNum(nThreadNum),
          m_nMaxThreadNum(nMaxThreadNum)
    {
    }

  public:
    CBlockingQueue<CTask *> m_taskQueue;
    CThreadGroup m_threadGroup;
    bool m_bExit;
    //当前线程数
    int m_nThreadNum;
    //线程池允许的最大线程数量
    int m_nMaxThreadNum;
};

CRegExecThreadPool::CRegExecThreadPool(int nThreadNum, int nMaxThreadNum)
    : d_ptr(new CRegExecThreadPoolPrivate(nThreadNum, nMaxThreadNum))
{
}

CRegExecThreadPool::CRegExecThreadPool()
    : d_ptr(new CRegExecThreadPoolPrivate(0, 0))
{
}

CRegExecThreadPool::~CRegExecThreadPool()
{
    Stop();

    if (d_ptr != NULL)
    {
        delete d_ptr;
        d_ptr = NULL;
    }
}

bool CRegExecThreadPool::Start()
{
    CL_D(CRegExecThreadPool);
    if (d->m_nMaxThreadNum <= 0)
    {
        return false;
    }

    d->m_bExit = false;
    int nCurrentStartThreadNum = d->m_threadGroup.size();
    for (int i = nCurrentStartThreadNum; (i < d->m_nThreadNum) && (i < d->m_nMaxThreadNum); ++i)
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
    for (; !d->m_bExit;)
    {
        CTask *pTask = d->m_taskQueue.TakeFromQueue();
        if (NULL != pTask)
        {
            pTask->Exec();
            delete pTask;
            pTask = NULL;
        }
    }
}

bool CRegExecThreadPool::PostTask(CTask *pTask)
{
    CL_D(CRegExecThreadPool);
    if (d->m_bExit)
    {
        if (NULL != pTask)
        {
            delete pTask;
            pTask = NULL;
        }

        return false;
    }

    d->m_taskQueue.AddToQueue(pTask);

    return true;
}

bool CRegExecThreadPool::AddThreadNum(int nThreadNum)
{
    CL_D(CRegExecThreadPool);
    d->m_nThreadNum += nThreadNum;
    if (d->m_nThreadNum > d->m_nMaxThreadNum)
    {
        d->m_nThreadNum = d->m_nMaxThreadNum;
    }
    return Start();
}

int CRegExecThreadPool::GetThreadNum()
{
    CL_D(CRegExecThreadPool);
    return d->m_nThreadNum;
}

void CRegExecThreadPool::SetMaxThreadNum(int nMaxThreadNum)
{
    CL_D(CRegExecThreadPool);
    d->m_nMaxThreadNum = nMaxThreadNum;
}

int CRegExecThreadPool::GetQueueSize() const
{
    CL_D(const CRegExecThreadPool);
    return d->m_taskQueue.GetQueueSize();
}


bool CRegExecThreadPool::IsAbleToAddThread() const
{
    CL_D(const CRegExecThreadPool);
    return (d->m_nThreadNum == d->m_nMaxThreadNum);
}

///////////////////////////////////////////////////////////////////////////////
typedef std::unordered_map<std::string, CTimerMessageLoop *> mapMsgLoop;
typedef mapMsgLoop::iterator mapMsgLoopIter;
typedef mapMsgLoop::const_iterator mapConstMsgLoopIter;

class CCachedThreadPoolPrivate
{
  public:
    CCachedThreadPoolPrivate()
        : m_bExit(true)
    {
    }

    CTimerMessageLoop *FindMsgLoop(const std::string &strID) const
    {
        CRMutexLock lock(m_mutex, std::defer_lock);
        mapConstMsgLoopIter iter = m_mapMsgLoop.find(strID);
        if (iter != m_mapMsgLoop.end())
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
    if (NULL != d_ptr)
    {
        delete d_ptr;
        d_ptr = NULL;
    }
}

bool CCachedThreadPool::Start()
{
    CL_D(CCachedThreadPool);

    CWMutexLock lock(d->m_mutex);
    if (!d->m_bExit)
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
    for (auto iter = d->m_mapMsgLoop.begin(); iter != d->m_mapMsgLoop.end();)
    {
        CTimerMessageLoop *pMsgLoop = iter->second;
        if (NULL == pMsgLoop)
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

bool CCachedThreadPool::PostTask(const std::string &strID, CTask *pTask)
{
    CL_D(CCachedThreadPool);

    {
        CRMutexLock lock(d->m_mutex, std::defer_lock);
        if (d->m_bExit)
        {
            if (NULL != pTask)
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
        if (iter != d->m_mapMsgLoop.end())
        {
            CTimerMessageLoop *pMsgLoop = iter->second;
            if (NULL == pMsgLoop)
            {
                if (NULL != pTask)
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
    for (auto iter = d->m_mapMsgLoop.begin(); iter != d->m_mapMsgLoop.end(); ++iter)
    {
        CTimerMessageLoop *pMsgLoop = iter->second;
        if (NULL == pMsgLoop)
        {
            if (NULL != pTask)
            {
                delete pTask;
                pTask = NULL;
            }

            assert(NULL);
            return false;
        }

        //存在空闲线程队列，将任务塞入该队列，并重置队列标识码
        if (pMsgLoop->IsIdeal())
        {
            d->m_mapMsgLoop.erase(iter);
            d->m_mapMsgLoop.insert(make_pair(strID, pMsgLoop));
            return pMsgLoop->PostTask(pTask);
        }
    }

    //不存在空闲，创建队列
    CTimerMessageLoop *pMsgLoop = new CTimerMessageLoop();
    pMsgLoop->Start();
    d->m_mapMsgLoop.insert(make_pair(strID, pMsgLoop));
    return pMsgLoop->PostTask(pTask);
}

void CCachedThreadPool::ClearTask(const std::string &strID)
{
    CL_D(CCachedThreadPool);

    CTimerMessageLoop *pMsgLoop = d->FindMsgLoop(strID);
    if (NULL == pMsgLoop)
    {
        return;
    }

    pMsgLoop->ClearTask();
}

void CCachedThreadPool::AsyncClearTask(const std::string &strID)
{
    CL_D(CCachedThreadPool);

    CTimerMessageLoop *pMsgLoop = d->FindMsgLoop(strID);
    if (NULL == pMsgLoop)
    {
        return;
    }

    pMsgLoop->AsyncClearTask();
}
} // namespace Commonlib