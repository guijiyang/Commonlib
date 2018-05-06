#include "TimerMessageLoop.h"
#include "Commonlib/MessageLoop/Private/TimerQueue.h"
#include "Commonlib/MessageLoop/Task.h"
#include "Commonlib/Thread/Mutex.h"
#include <thread>

namespace Commonlib {
class CTimerMessageLoopPrivate {
public:
  CTimerMessageLoopPrivate() : m_bExit(true), m_bIdeal(true) {}

public:
  std::thread m_taskThread; //任务执行线程
  bool m_bExit;             //线程状态，是否退出
  bool m_bIdeal;            //线程状态，是否空闲
  CMutex m_mutex;
  CTimerQueue<CTask *> m_taskQueue; //任务执行队列
};

CTimerMessageLoop::CTimerMessageLoop()
    : d_ptr(new CTimerMessageLoopPrivate()) {}

CTimerMessageLoop::~CTimerMessageLoop() {
  Stop();

  if (NULL != d_ptr) {
    delete d_ptr;
    d_ptr = NULL;
  }
}

bool CTimerMessageLoop::IsIdeal() const {
  CL_D(const CTimerMessageLoop);
  return d->m_bIdeal;
}

bool CTimerMessageLoop::PostTask(CTask *pTask) {
  CL_D(CTimerMessageLoop);
  if (d->m_bExit) {
    if (NULL != pTask) {
      delete pTask;
      pTask = NULL;
    }
    return false;
  }

  d->m_taskQueue.AddToQueue(pTask);
  d->m_bIdeal = false;
  return true;
}

void CTimerMessageLoop::ClearTask() {
  CL_D(CTimerMessageLoop);
  std::vector<CTask *> taskQueue;
  d->m_taskQueue.TakeAllFromQueue(taskQueue);
  for (std::vector<CTask *>::iterator iter = taskQueue.begin();
       iter != taskQueue.end();) {
    CTask *pTask = *iter;
    if (NULL != pTask) {
      delete pTask;
      pTask = NULL;
    }
    iter = taskQueue.erase(iter);
  }

  CMutexLock lock(d->m_mutex);
  d->m_bIdeal = true;
}

void CTimerMessageLoop::AsyncClearTask() {
  CL_D(CTimerMessageLoop);
  std::vector<CTask *> taskQueue;
  d->m_taskQueue.TakeAllFromQueue(taskQueue);
  for (std::vector<CTask *>::iterator iter = taskQueue.begin(); iter != taskQueue.end();) 
  {
      CTask *pTask = *iter;
      if (NULL != pTask) 
      {
        delete pTask;
        pTask = NULL;
      }
    iter = taskQueue.erase(iter);
  }
}

bool CTimerMessageLoop::Start() {
  CL_D(CTimerMessageLoop);

  if (!d->m_bExit) {
    //线程在执行的情况下
    return false;
  }

  if (d->m_taskThread.joinable()) {
    return false;
  }

  d->m_bExit = false;
  d->m_bIdeal = false;
  std::function<void(void)> func = std::bind(&CTimerMessageLoop::Run, this);
  d->m_taskThread = std::thread(func);

  return true;
}

bool CTimerMessageLoop::Stop() {
  CL_D(CTimerMessageLoop);
  d->m_bExit = true;
  d->m_bIdeal = true;
  d->m_taskQueue.NotifyAll();
  d->m_taskThread.join();

  return true;
}

bool CTimerMessageLoop::IsRunning() const {
  CL_D(const CTimerMessageLoop);
  return !d->m_bExit;
}

void CTimerMessageLoop::Run() {
  CL_D(CTimerMessageLoop);

  for (; !d->m_bExit;) {
    CTask *pTask = d->m_taskQueue.TakeFromQueue();
    if (NULL != pTask) {
      CMutexLock lock(d->m_mutex);
      if (!d->m_bIdeal) {
        pTask->Exec();
      }

      delete pTask;
      pTask = NULL;
    } else {
      //退出情况不考虑
      if (d->m_bExit) {
        break;
      }

      //超时销毁
      if (!d->m_taskQueue.IsEmpty()) {
        break;
      }

      d->m_bIdeal = true;
      d->m_taskQueue.Wait();
    }
  }

  //退出线程前，先销毁队列
  while (!d->m_taskQueue.IsEmpty()) {
    CTask *pTask = d->m_taskQueue.TakeFromQueue();
    if (NULL != pTask) {
      delete pTask;
      pTask = NULL;
    }
  }
}
} // namespace Commonlib
