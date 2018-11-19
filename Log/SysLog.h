#ifndef SYSLOG_H
#define SYSLOG_H

// #include "Pattern/CSingleton.h"
#include "CommonGlobal.h"
#include <syslog.h>
#include <string>

namespace Commonlib
{
class COMMON_EXPORT CSysLog
{
protected:
  CSysLog();
  ~CSysLog();

public:
  void OpenLog(const std::string &strProgamName = "", int nOption = LOG_NDELAY | LOG_PID, int nFacility = LOG_USER);
  void CloseLog()
  {
    closelog();
  }
  void WriteLogWithTime(int level, const std::string &strTime, const std::string &strMessage);

public:
  virtual void WriteLog(int level, const std::string &strMessage);
};
} // namespace Commonlib

#endif //SYSLOG_H