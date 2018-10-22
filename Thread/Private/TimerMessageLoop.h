#ifndef COMMONLIB_TIMERMESSAGELOOP_H
#define COMMONLIB_TIMERMESSAGELOOP_H

#include "CommonGlobal.h"

namespace Commonlib
{
    class CTimerMessageLoopPrivate;
    class CTask;

    //单线程消息循环
    class CTimerMessageLoop
    {
    public:
        //是否是空闲的
        bool IsIdeal() const;
        //塞入队列
        bool PostTask(CTask* pTask);
        //清除任务
        void ClearTask();
        //异步清除任务
        void AsyncClearTask();
    public:
        //启动消息队列
        bool Start();
        //停止消息队列
        bool Stop();
        //消息队列是否启动
        bool IsRunning() const;

    public:
        CTimerMessageLoop();
        ~CTimerMessageLoop();

    protected:
        void Run();

    private:
        CL_DECLARE_PRIVATE(CTimerMessageLoop);
        CTimerMessageLoopPrivate* d_ptr;
    };
}

#endif //COMMONLIB_TIMERMESSAGELOOP_H