#ifndef COMMONLIB_THREADSAFEASSERT_H
#define COMMONLIB_THREADSAFEASSERT_H

#include "Commonlib/CommonGlobal.h"

namespace Commonlib
{
#ifdef _DEBUG
class CThreadSafeAssertPrivate;
class COMMON_EXPORT CThreadSafeAssert
{
  public:
	CThreadSafeAssert();
	~CThreadSafeAssert();

  protected:
	// 非线程安全接口，会出断言
	void CallOnNonThreadSafe();

  private:
	DS_DECLARE_PRIVATE(cthreadsafeassert);
	CThreadSafeAssertPrivate *d_ptr;
};
#else
class COMMON_EXPORT CThreadSafeAssert
{
  public:
	CThreadSafeAssert();
	~CThreadSafeAssert();

  protected:
	// 非线程安全接口，会出断言
	void CallOnNonThreadSafe();
};
#endif
} // namespace Commonlib

#endif //COMMONLIB_THREADSAFEASSERT_H