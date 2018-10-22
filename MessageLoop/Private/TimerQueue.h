#ifndef COMMONLIB_TIMERQUEUE_H
#define COMMONLIB_TIMERQUEUE_H

#include <deque>
#include <vector>
#include <condition_variable>
#include <chrono>

#include "Thread/Mutex.h"
#include "Noncopyable.h"

namespace Commonlib
{
	template<class T>
	class CTimerQueue : private CNoncopyable
	{
	public:
		void AddToQueue(const T& x)
		{
			CMutexLock lock(m_mutex);
			m_deque.push_front(x);
			m_condition.notify_one();
		}

		T TakeFromQueue()
		{
			CMutexLock lock(m_mutex);
			if(m_deque.empty())
			{
				m_condition.wait_for(lock,
					m_nKeepAliveTime*std::chrono::seconds(1)); 
			}

			if(m_deque.empty())
			{
				return NULL;
			}
			T front(m_deque.front());
			m_deque.pop_front();
			return front;
		}

		void TakeAllFromQueue(std::vector<T>& vect)
		{
			CMutexLock lock(m_mutex);
			while(!m_deque.empty())
			{
				vect.push_back(m_deque.front());
				m_deque.pop_front();
			}
		}

		void Wait()
		{
			CMutexLock lock(m_mutex);
			m_condition.wait(lock);
		}

		void NotifyAll()
		{
			m_condition.notify_all();
		}

		bool IsEmpty() const
		{
			CMutexLock lock(m_mutex);
			return m_deque.empty();
		}

	private:
		mutable CMutex 					m_mutex;
		std::deque<T>			m_deque;
		std::condition_variable	m_condition;
		unsigned int			m_nKeepAliveTime;		//线程挂起时间 (S)
	};
}
#endif
