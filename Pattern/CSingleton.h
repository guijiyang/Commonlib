#ifndef COMMONLIB_CSINGLETON_H
#define COMMONLIB_CSINGLETON_H

#include "Commonlib/Thread/Mutex.h"

#include <memory>

namespace Commonlib {
/**
 * 单件类
 **/
template <class T> class CSingleton {
public:
  //获取单件
  static T &GetInstance();

  //销毁单件--智能指针不需要手动释放
  // static void DestroyInstance();

protected:
  //构造
  CSingleton() {}
  //析构
  virtual ~CSingleton() {}

private:
  //拷贝构造
  CSingleton(const CSingleton &);
  //复制
  CSingleton &operator=(const CSingleton &);
};

//获取单件
template <class T> inline T &CSingleton<T>::GetInstance() {
  static std::unique_ptr<T> s_ptrInstance;
  static CMutex m_mutexInstance;

  if (NULL == s_ptrInstance.get()) {
    CMutexLock lock(m_mutexInstance);
    if (NULL == s_ptrInstance.get()) {
      s_ptrInstance = std::unique_ptr<T>(new T);
    }
  }

  return *s_ptrInstance.get();
}

//销毁单件--智能指针不需要手动释放
// template<class T>
// inline void CSingleton<T>::DestroyInstance()
// {
//     if(NULL != s_ptrInstance.get())
//     {
//         CMutexLock lock(m_mutexInstance);
//         if(NULL != s_ptrInstance.get())
//         {
//             delete s_ptrInstance；
//             s_ptrInstance = NULL;
//         }
//     }
// }
} // namespace Commonlib

#endif // COMMONLIB_CSINGLETON_H