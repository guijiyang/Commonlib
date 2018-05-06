#ifndef COMMONLIB_CTASK_H
#define COMMONLIB_CTASK_H

#include "Commonlib/CommonGlobal.h"
#include <functional>

namespace Commonlib
{
    class CFunctionTaskPrivate;

    class COMMON_EXPORT CTask
    {
    public:
        CTask(){}
        virtual ~CTask(){}

    public:
        virtual void Exec() = 0;
    };

    class COMMON_EXPORT CFunctionTask : public CTask
    {
    public:
        CFunctionTask(std::function<void()> func);

        virtual ~CFunctionTask();

    public:
        virtual void Exec();

    private:
        CL_DECLARE_PRIVATE(CFunctionTask);
        CFunctionTaskPrivate* d_ptr;
    };
}

#endif //COMMONLIB_CTASK_H