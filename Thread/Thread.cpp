#include "Thread.h"
#include "Thread/Mutex.h"

#include <functional>
#include <thread>
#include <chrono>

namespace Commonlib
{
class CThreadPrivate
{
  public:
    CThreadPrivate()
        : m_bExit(true),
          m_msWaitTime(std::chrono::milliseconds(0))
    {
    }

  public:
    std::thread m_thread;
    bool m_bExit;
    CMutex m_mutex;
    std::condition_variable m_condition;
    std::chrono::milliseconds m_msWaitTime;
};

CThread::CThread()
    : d_ptr(new CThreadPrivate())
{
}

CThread::~CThread()
{
    StopThread();

    if (NULL != d_ptr)
    {
        delete d_ptr;
        d_ptr = NULL;
    }
}

bool CThread::StartThread(unsigned int uWaitTime)
{
    CL_D(CThread);
    if (d->m_thread.joinable())
    {
        return false;
    }

    d->m_bExit = false;
    d->m_msWaitTime = std::chrono::milliseconds(uWaitTime);

    std::function<void(void)> func = std::bind(&CThread::CallThread, this);
    d->m_thread = std::thread(func);

    return true;
}

void CThread::StopThread()
{
    CL_D(CThread);
    d->m_bExit = true;
    d->m_condition.notify_all();
    d->m_thread.join();
}

//唤醒线程，取消等待
void CThread::WakeUpOnce()
{
    CL_D(CThread);
    d->m_condition.notify_all();
}

//线程是否运行
bool CThread::IsRunning() const
{
    CL_D(const CThread);
    return !d->m_bExit;
}

void CThread::CallThread()
{
    CL_D(CThread);
    CMutexLock lock(d->m_mutex);
    while (!d->m_bExit)
    {
        if (!Run())
        {
            break;
        }

        if (d->m_msWaitTime != std::chrono::milliseconds(0))
        {
            if (!d->m_bExit)
            {
                d->m_condition.wait_for(lock, d->m_msWaitTime);
            }
        }
    }
}

//线程睡眠
void CThread::Sleep(unsigned long iSec)
{
    std::this_thread::sleep_for(std::chrono::seconds(iSec));
}

void CThread::MSleep(unsigned long iMSec)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(iMSec));
}

void CThread::USleep(unsigned long iUSec)
{
    std::this_thread::sleep_for(std::chrono::microseconds(iUSec));
}
} // namespace Commonlib
