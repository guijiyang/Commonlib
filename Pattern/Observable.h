#ifndef COMMONLIB_OBSERVABLE_H
#define COMMONLIB_OBSERVABLE_H

#include "Thread/Mutex.h"
#include "CommonGlobal.h"

#include <list>

namespace Commonlib
{
    template<class CObserver>
    class CObservable
    {
        typedef typename std::list<CObserver*>::iterator CObserverIterator;
    public:
        CObservable();
        ~CObservable();

    public:
        //注册
        bool RegisterObserver(CObserver*);
        //销毁
        bool UnRegisterObserver(COBserver*)

    public:
        //通知观察者
        template<typename pFunction, typename Param>
        void NotifyObservers(pFunction fn, const Param& pParam);

        template<typename pFunction, typename Param>
        void NotifyObservers(pFunction fn, const Param* pParam); 

    private:
        std::list<CObserver*>   m_lstObservers;           //观察者列表
        CMutex                  m_mutex;                  //互斥量    
    };

    template<class CObserver>
    inline CObservable<CObserver>::CObservable()
    {
    }

    template<class CObserver>
    inline CObservable<CObserver>::~CObservable()
    {
    }

    //注册
    template<class CObserver>
    inline bool CObservable<CObserver>::RegisterObserver(CObserver* pObserver)
    {
        if(NULL == pObserver)
        {
            return false;
        }

        CMutexLock lock(m_mutex);
        m_lstObservers.push_front(pObserver);

#ifdef _DEBUG
        pObserver->IncreaseCount();
#endif

        return true;
    }

    //注销
    template<class CObserver>
    inline bool CObservable<CObserver>::UnRegisterObserver(CObserver* pObserver)
    {
        if(NULL == pObserver)
        {
            return false;
        }

        CMutexLock lock(m_mutex);
        CObserverIterator iter = find(m_lstObservers.begin(), m_lstObservers.end(), pObserver);
        if(iter != m_lstObservers.end())
        {
#ifdef _DEBUG
            (*iter)->DecreaseCount();
#endif            
        }

        return true;
    }

    //通知所有观察者
    template<typename pFunction, typename pParam>
    void CObservable::NotifyObservers(pFunction fn, const Param& pParam)
    {
        CMutexLock lock(m_mutex);
        CObserverIterator IterlstBegin = m_lstObservers.begin();
        CObserverIterator IterlstEnd = m_lstObservers.end();
        for(CObserverIterator iter = IterlstBegin; iter != IterlstEnd; ++it)
        {
            CObserver* pCObserver = *iter;
            (pCObserver->*fn)(pParam);
        }
    }

    //通知所有观察者
    template<typename pFunction, typename pParam>
    void CObservable::NotifyObservers(pFunction fn, const Param* pParam)
    {
        CMutexLock lock(m_mutex);
        CObserverIterator IterlstBegin = m_lstObservers.begin();
        CObserverIterator IterlstEnd = m_lstObservers.end();
        for(CObserverIterator iter = IterlstBegin; iter != IterlstEnd; ++it)
        {
            CObserver* pCObserver = *iter;
            (pCObserver->*fn)(pParam);
        }
    }
}
#endif //COMMONLIB_OBSERVABLE_H
