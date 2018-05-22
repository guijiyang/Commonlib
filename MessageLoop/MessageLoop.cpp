#include "MessageLoop.h"
#include "Commonlib/MessageLoop/Private/BlockingQueue.h"
#include "Commonlib/MessageLoop/Task.h"
#include "Commonlib/Thread/Mutex.h"

#include <thread>

namespace Commonlib
{
class CBaseMessageLoopPrivate
{
public:
  CBaseMessageLoopPrivate() : m_Exit(true) {}

public:
  bool m_Exit;                          //任务线程执行状态。是否停止
  std::thread m_taskThread;             //任务执行线程
  CBlockingQueue<CTask *> m_taskQueues; //任务队列
};

CBaseMessageLoop::CBaseMessageLoop() : d_ptr(new CBaseMessageLoopPrivate()) {}

CBaseMessageLoop::~CBaseMessageLoop()
{
  Stop();

  if (NULL != d_ptr)
  {
    delete d_ptr;
    d_ptr = NULL;
  }
}

bool CBaseMessageLoop::Start()
{
  CL_D(CBaseMessageLoop);

  if (d->m_taskThread.joinable())
  {
    return false;
  }

  d->m_Exit = false;
  std::function<void(void)> func = std::bind(&CBaseMessageLoop::Run, this);
  d->m_taskThread = std::thread(func);

  return true;
}

void CBaseMessageLoop::Stop()
{
  CL_D(CBaseMessageLoop);
  d->m_taskQueues.NotifyAll();
  d->m_Exit = true;
  d->m_taskThread.join();
}

bool CBaseMessageLoop::PostTask(CTask *pTask)
{
  CL_D(CBaseMessageLoop);

  if (d->m_Exit)
  {
    if (NULL != pTask)
    {
      delete pTask;
      pTask = NULL;
    }
    return false;
  }

  d->m_taskQueues.AddToQueue(pTask);

  return true;
}

void CBaseMessageLoop::Run()
{
  CL_D(CBaseMessageLoop);

  for (; !d->m_Exit;)
  {
    CTask *pTask = d->m_taskQueues.TakeFromQueue();
    if (NULL != pTask)
    {
      pTask->Exec();
      delete pTask;
      pTask = NULL;
    }
  }

  //任务线程结束前，清空剩余未处理的消息队列
  while (!d->m_taskQueues.IsEmpty())
  {
    CTask *pTask = d->m_taskQueues.TakeFromQueue();
    if (NULL != pTask)
    {
      delete pTask;
      pTask = NULL;
    }
  }
}

bool CBaseMessageLoop::IsRunning() const
{
  CL_D(const CBaseMessageLoop);
  return !d->m_Exit;
}
} // namespace Commonlib