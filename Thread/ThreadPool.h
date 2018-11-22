#ifndef COMMONLIB_THREADPOOL_H
#define COMMONLIB_THREADPOOL_H

#include "CommonGlobal.h"
#include "Pattern/CSingleton.h"

namespace Commonlib
{
class CTask;
//定时执行无顺序消息循环
class CRegExecThreadPoolPrivate;
class CRegExecThreadPool
{
  public:
    //塞入队列
    bool PostTask(CTask *pTask);

  public:
    //启动消息循环线程池
    bool Start();
    //停止消息循环线程池
    void Stop();
    //增加线程池数量
    bool AddThreadNum(int nThreadNum);
    //返回线程数
    int GetThreadNum();
    //设置最大线程数,通常是在线程池工作前设置
    void SetMaxThreadNum(int nMaxThreadNum);
    //线程工作状态
    bool IsRunning() const;
    //获取等待处理的任务数量
    int GetQueueSize() const;
    //判断当前线程数是否等于最大的线程数，是返回false，否则返回true
    bool IsAbleToAddThread() const;


  protected:
    void Run();

  protected:
    CRegExecThreadPool();
    CRegExecThreadPool(int nThreadNum, int nMaxThreadNum);
    ~CRegExecThreadPool();

  private:
    CL_DECLARE_PRIVATE(CRegExecThreadPool);
    CRegExecThreadPoolPrivate *d_ptr;
};

class CCachedThreadPoolPrivate;
//可缓存的线程池
class COMMON_EXPORT CCachedThreadPool
{
    friend class CMessageLoopPoolForSDK;

  public:
    // 塞入任务
    // @param[in] const std::string& strID 线程池队列标识码，相同的队列需保证唯一
    // @param[in] CTask* pTask 任务
    // @return
    bool PostTask(const std::string &strID, CTask *pTask);

    // 置为空闲且清除队列中的任务
    // 等待当前正在执行任务完毕
    // @param[in] const std::string& strID 线程池队列标识码，相同的队列需保证唯一
    // @return
    void ClearTask(const std::string &strID);

    // 异步置为空闲并清除队列中的任务
    // 不清除当前正在执行的任务
    // @param[in] const std::string& strID 线程池队列标识码，相同的队列需保证唯一
    // @return
    void AsyncClearTask(const std::string &strID);

  public:
    //启动
    bool Start();
    //停止
    void Stop();

  private:
    CCachedThreadPool();
    ~CCachedThreadPool();

  private:
    CL_DECLARE_PRIVATE(CCachedThreadPool);
    CCachedThreadPoolPrivate *d_ptr;
};

//SDK同步调用队列。对于需持续使用的，例如登录心跳等，不适用此线程
class COMMON_EXPORT CMessageLoopPoolForSDK : public CCachedThreadPool,
                                             public CSingleton<CMessageLoopPoolForSDK>
{
    friend class CSingleton<CMessageLoopPoolForSDK>;

    // private:
    //     CMessageLoopPoolForSDK();
};

} // namespace Commonlib

#endif //COMMONLIB_THREADPOOL_H