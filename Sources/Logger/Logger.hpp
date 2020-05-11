#pragma once
#include <string>
#include <fstream>
#include <boost/date_time.hpp>

struct Logger
{
    enum class LogType
    {
        Debug,
        Error
    };

    void Log(const std::string& aNewLog, LogType aLogType)
    {
        switch (aLogType)
        {
            case LogType::Debug:
                DebugLog(aNewLog);
            case LogType::Error:
                ErrorLog(aNewLog);
            default:
                assert(false && "Unknown logType");
        }
    }

    Logger()
    {
        mLogFile.open("log.txt", std::fstream::app);
        mLogFile << "\n=== Begin session " << boost::posix_time::second_clock::local_time() << " ===\n";
    }

    ~Logger()
    {
        mLogFile.close();
    }

private:

    void DebugLog(const std::string& aNewLog)
    {
        mLogFile << boost::posix_time::second_clock::local_time() << ": " << aNewLog << std::endl;
    }

    void ErrorLog(const std::string& aNewLog)
    {
        mLogFile << boost::posix_time::second_clock::local_time() << ": [!] " << aNewLog << std::endl;
    }

    std::fstream mLogFile;
};

Logger& GetLogger()
{
    static Logger object;
    return object;
}