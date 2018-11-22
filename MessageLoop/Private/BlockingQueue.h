#ifndef COMMONLIB_BLOCKINGQUEUE_H
#define COMMONLIB_BLOCKINGQUEUE_H

#include <condition_variable>
#include <deque>

#include "Noncopyable.h"
#include "Thread/Mutex.h"

namespace Commonlib
{
template <class T>
class CBlockingQueue : private CNoncopyable
{
public:
  void AddToQueue(const T &x)
  {
    CMutexLock lock(m_mutexQueue);
    m_deque.push_back(x);
    m_condition.notify_one();
  }

  T TakeFromQueue()
  {
    CMutexLock lock(m_mutexQueue);
    if (m_deque.empty())
    {
      m_condition.wait(lock);
    }

    if (m_deque.empty())
    {
      return NULL;
    }

    T front(m_deque.front());
    m_deque.pop_front();
    return front;
  }

  bool IsEmpty() const
  {
    CMutexLock lock(m_mutexQueue);
    return m_deque.empty();
  }

  int GetQueueSize() const
  {
    return m_deque.size();
  }

  void NotifyAll() { m_condition.notify_all(); }

private:
  mutable CMutex m_mutexQueue;
  std::condition_variable m_condition; //条件变量
  std::deque<T> m_deque;
};
} // namespace Commonlib
#endif // COMMONLIB_BLOCKINGQUEUE_H