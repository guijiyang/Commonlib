#ifndef COMMONLIB_MUTEX_H
#define COMMONLIB_MUTEX_H

#include <mutex>
#include <shared_mutex>

namespace Commonlib {
////////////////////
//独占锁
///////////////////
typedef std::mutex CMutex;                       //互斥量
typedef std::unique_lock<std::mutex> CMutexLock; //独占锁lock

//////////////////
//读写锁
//////////////////
//读写锁共享互斥量 shared_mutex = C++17; shared_timed_mutex = c++14
#if __cplusplus > 201402L
typedef std::shared_mutex CWRMutex;
#else
#if __cplusplus > 201103L
typedef std::shared_timed_mutex CWRMutex;
//读锁
typedef std::shared_lock<CWRMutex> CRMutexLock;
#endif
#endif
//写锁
typedef std::unique_lock<CWRMutex> CWMutexLock;

//////////////////
//递归锁
/////////////////
typedef std::recursive_mutex CRecursiveMutex;
} // namespace Commonlib
#endif // COMMONLIB_MUTEX_H