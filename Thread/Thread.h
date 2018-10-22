#ifndef COMMONLIB_THREAD_H
#define COMMONLIB_THREAD_H

#include "CommonGlobal.h"

namespace Commonlib
{
class CThreadPrivate;

//线程
class COMMON_EXPORT CThread
{
public:
  //启动线程，单位毫秒
  bool StartThread(unsigned int uWaitTime);

  //停止线程
  void StopThread();

  //唤起线程，取消等待
  void WakeUpOnce();

  //线程是否运行
  bool IsRunning() const;

public:
  //线程睡眠
  static void Sleep(unsigned long iSecs);
  static void MSleep(unsigned long iMSecs);
  static void USleep(unsigned long iUSecs);

protected:
  CThread();
  ~CThread();

protected:
  //线程执行。返回值：false：停止线程函数 true：正常执行
  virtual bool Run() = 0;

private:
  //启动线程
  void CallThread();

private:
  CL_DECLARE_PRIVATE(CThread);
  CThreadPrivate *d_ptr;
};
} // namespace Commonlib

#endif // COMMONLIB_THREAD_H