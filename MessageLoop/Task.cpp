#include "Task.h"

namespace Commonlib
{
    class CFunctionTaskPrivate
    {
    public:
        CFunctionTaskPrivate(std::function<void()> func)
            :m_function(func)
        {
        }

    public:
        std::function<void()> m_function;
    };

    CFunctionTask::CFunctionTask(std::function<void()> func)
        :d_ptr(new CFunctionTaskPrivate(func))
    {
    }

    CFunctionTask::~CFunctionTask()
    {
        if(d_ptr != NULL)
        {
            delete d_ptr;
            d_ptr = NULL;
        }
    }

    void CFunctionTask::Exec()
    {
        CL_D(CFunctionTask);
        if(NULL == d->m_function)
        {
            return;
        }

        std::function<void()> function = d->m_function;
        function();
    }
}