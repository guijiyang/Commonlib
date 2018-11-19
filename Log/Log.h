#ifndef COMMONLIB_LOG_H
#define COMMONLIB_LOG_H

#include "CommonGlobal.h"
#include "Pattern/CSingleton.h"

#include <log4cxx/logger.h>
namespace Commonlib
{
//打印日志宏
//调试信息
#define LOG_DEBUG(strLogName, strMessage, ...)                               \
    {                                                                        \
        CLog::GetInstance().Debug(strLogName, strMessage, LOG4CXX_LOCATION); \
    }

//提示
#define LOG_INFO(strLogName, strMessage, ...)                         \
    {                                                                 \
    CLog:                                                             \
        GetInstance().Info(strLogName, strMessage, LOG4CXX_LOCATION); \
    }

//警告
#define LOG_WARN(strLogName, strMessage, ...)                         \
    {                                                                 \
    CLog:                                                             \
        GetInstance().Warn(strLogName, strMessage, LOG4CXX_LOCATION); \
    }

//错误
#define LOG_ERROR(strLogName, strMessage, ...)                         \
    {                                                                  \
    CLog:                                                              \
        GetInstance().Error(strLogName, strMessage, LOG4CXX_LOCATION); \
    }

class COMMON_EXPORT CLog : public CSingleton<CLog>
{
    friend class CSingleton<CLog>;

  private:
    CLog();
    ~CLog();

  public:
    //调试
    void Debug(const std::string &strLogName, const std::string &strMessage,
               const log4cxx::spi::LocationInfo &logInfo);

    //提示
    void Info(const std::string &strLogName, const std::string &strMessage,
              const log4cxx::spi::LocationInfo &logInfo);

    //警告
    void Warn(const std::string &strLogName, const std::string &strMessage,
              const log4cxx::spi::LocationInfo &logInfo);

    //错误
    void Error(const std::string &strLogName, const std::string &strMessage,
               const log4cxx::spi::LocationInfo &logInfo);
};

} // namespace Commonlib
#endif //COMMONLIB_LOG_H