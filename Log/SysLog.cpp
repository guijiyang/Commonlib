#include "SysLog.h"

namespace Commonlib
{
CSysLog::CSysLog()
{
}

CSysLog::~CSysLog()
{
    CloseLog();
}

void CSysLog::OpenLog(const std::string &strProgamName, int nOption, int nFacility)
{
    openlog(strProgamName.data(), nOption, nFacility);
}

void CSysLog::WriteLog(int level, const std::string &strMessage)
{
    WriteLogWithTime(level, "", strMessage);
}

void CSysLog::WriteLogWithTime(int level, const std::string &strTime, const std::string &strMessage)
{
    std::string strLogMsg = strTime;
    switch (level)
    {
    case LOG_EMERG:
        strLogMsg += " [EMERG] " + strMessage;
        break;
    case LOG_ALERT:
        strLogMsg += " [ALERT] " + strMessage;
        break;
    case LOG_CRIT:
        strLogMsg += " [CRITICAL] " + strMessage;
        break;
    case LOG_ERR:
        strLogMsg += " [ERROR] " + strMessage;
        break;
    case LOG_WARNING:
        strLogMsg += " [WARNING] " + strMessage;
        break;
    case LOG_NOTICE:
        strLogMsg += " [NOTICE] " + strMessage;
        break;
    case LOG_INFO:
        strLogMsg += " [INFO] " + strMessage;
        break;
    case LOG_DEBUG:
        strLogMsg += " [DEBUG] " + strMessage;
        break;
    default:
        strLogMsg += strMessage;
    }
    syslog(level, "%s", strLogMsg.data());
}
} // namespace Commonlib
