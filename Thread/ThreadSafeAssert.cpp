#include "ThreadSafeAssert.h"
#include <thread>

namespace Commonlib
{
#ifdef _DEBUG
class CThreadSafeAssertPrivate
{
  public:
    CThreadSafeAssertPrivate()
    {
    }

  public:
    std::thread::id m_idCurThread;
};

CThreadSafeAssert::CThreadSafeAssert()
    : d_ptr(new CThreadSafeAssertPrivate())
{
    d_ptr->m_idCurThread = std::thread::get_id();
}

CThreadSafeAssert::~CThreadSafeAssert()
{
    if (NULL != d_ptr)
    {
        delete d_ptr;
        d_ptr = NULL;
    }
}

//非线程安全接口,会出断言
void CThreadSafeAssert::CallOnNonThreadSafe()
{
    CL_D(CThreadSafeAssert);
    assert(d->m_idCurThread == std::thread::get_id());
}

#else
CThreadSafeAssert::CThreadSafeAssert()
{
}

CThreadSafeAssert::~CThreadSafeAssert()
{
}

//非线程安全接口,会出断言
void CThreadSafeAssert::CallOnNonThreadSafe()
{
}
#endif
} // namespace Commonlib
