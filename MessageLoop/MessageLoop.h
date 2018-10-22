#ifndef COMMONLIB_MESSAGELOOP_H
#define COMMONLIB_MESSAGELOOP_H

#include "CommonGlobal.h"
#include "Pattern/CSingleton.h"

namespace Commonlib
{
class CBaseMessageLoopPrivate;
class CTask;

//消息循环基类
class COMMON_EXPORT CBaseMessageLoop
{
public:
  //将消息塞入循环队列
  bool PostTask(CTask *pTask);

public:
  //开启消息循环线程
  bool Start();

  //关闭消息循环线程
  void Stop();

  //判断消息循环线程是否在运行
  bool IsRunning() const;

protected:
  CBaseMessageLoop();
  ~CBaseMessageLoop();

  //线程执行函数
  void Run();

private:
  CL_DECLARE_PRIVATE(CBaseMessageLoop);
  CBaseMessageLoopPrivate *d_ptr;
};

//消息循环实现类
class COMMON_EXPORT CMessageLoop : public CBaseMessageLoop,
                                   public CSingleton<CMessageLoop>
{
  friend class CSingleton<CMessageLoop>;

private:
  CMessageLoop()
      : CBaseMessageLoop() {} //必须将构造函数定义为私有，才能让子类实现单例模式
};
} // namespace Commonlib

#endif // COMMONLIB_MESSAGELOOP_H