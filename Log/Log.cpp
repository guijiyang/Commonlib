#include "Log.h"

#include <log4cxx/propertyconfigurator.h>

namespace Commonlib
{
    CLog::CLog()
    {
        log4cxx::PropertyConfigurator::configureAndWatch("Server.log4cxx.properties");
    }

    CLog::~CLog()
    {

    }

    void CLog::Debug(const std::string& strLogName, const std::string& strMessage, const log4cxx::spi::LocationInfo& logInfo)
    {
        log4cxx::LoggerPtr pLogger = log4cxx::Logger::getLogger(strLogName);
        if(pLogger->isDebugEnabled())
        {
            log4cxx::helpers::MessageBuffer messagebuffer;
            pLogger->forcedLog(log4cxx::Level::getDebug(), messagebuffer.str(messagebuffer << strMessage), logInfo);
        }
    }

    void CLog::Info(const std::string& strLogName, const std::string& strMessage, const log4cxx::spi::LocationInfo& logInfo)
    {
        log4cxx::LoggerPtr pLogger = log4cxx::Logger::getLogger(strLogName);
        if(pLogger->isInfoEnabled())
        {
            log4cxx::helpers::MessageBuffer messagebuffer;
            pLogger->forcedLog(log4cxx::Level::getInfo(), messagebuffer.str(messagebuffer << strMessage), logInfo);
        }
    }

    void CLog::Warn(const std::string& strLogName, const std::string& strMessage, const log4cxx::spi::LocationInfo& logInfo)
    {
        log4cxx::LoggerPtr pLogger = log4cxx::Logger::getLogger(strLogName);
        if(pLogger->isInfoEnabled())
        {
            log4cxx::helpers::MessageBuffer messagebuffer;
            pLogger->forcedLog(log4cxx::Level::getWarn(), messagebuffer.str(messagebuffer << strMessage), logInfo);
        }
    }

    void CLog::Error(const std::string& strLogName, const std::string& strMessage, const log4cxx::spi::LocationInfo& logInfo)
    {
        log4cxx::LoggerPtr pLogger = log4cxx::Logger::getLogger(strLogName);
        if(pLogger->isErrorEnabled())
        {
            log4cxx::helpers::MessageBuffer messagebuffer;
            pLogger->forcedLog(log4cxx::Level::getError(), messagebuffer.str(messagebuffer << strMessage), logInfo);
        }
    }
}