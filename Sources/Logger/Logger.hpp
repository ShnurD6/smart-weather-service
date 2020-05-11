#pragma once
#include <string>
#include <fstream>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

using namespace std::string_literals;

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
                break;
            case LogType::Error:
                ErrorLog(aNewLog);
                break;
            default:
                assert(false && "Unknown logType");
                break;
        }
    }

    Logger()
    {
        SetTargetFromEnv();

        const auto initialLog = (boost::format("\n=== Begin session %s ===\n")
            % boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time())).str();

        switch (mTarget)
        {
            case Target::File:
                mLogFile.open("log.txt", std::fstream::app);
                mLogFile << initialLog;
                break;
            case Target::OutputStream:
                std::cout << initialLog;
                break;
            default:
                assert(false && "Unknown logTarget");
                break;
        }
    }

    ~Logger()
    {
        switch (mTarget)
        {
            case Target::File:
                mLogFile.close();
                break;
            case Target::OutputStream:
                break;
            default:
                assert(false && "Unknown logTarget");
                break;
        }
    }

private:

    static constexpr auto mTargetEnvVar = "SWS_LOG_TARGET";

    enum class Target
    {
        File,        ///< Logging to a file log.txt
        OutputStream ///< Logging to std::cout/std::cerr
    };

    Target mTarget{Target::File};

    void SetTargetFromEnv()
    {
        const auto targetStr = std::getenv(mTargetEnvVar);

        if (!targetStr)
            return;

        if (targetStr == "File"s)
            mTarget = Target::File;
        else if (targetStr == "OutputStream"s)
            mTarget = Target::OutputStream;
    }

    void DebugLog(const std::string& aNewLog)
    {
        const auto log = (boost::format("%s: %s\n")
            % boost::posix_time::second_clock::local_time()
            % aNewLog);

        switch (mTarget)
        {
            case Target::File:
                mLogFile << log;
                break;
            case Target::OutputStream:
                std::cout << log;
                break;
            default:
                assert(false && "Unknown logTarget");
                break;
        }
    }

    void ErrorLog(const std::string& aNewLog)
    {
        const auto log = (boost::format("%s: [!] %s\n")
            % boost::posix_time::second_clock::local_time()
            % aNewLog);

        switch (mTarget)
        {
            case Target::File:
                mLogFile << log;
                break;
            case Target::OutputStream:
                std::cerr << log;
                break;
            default:
                assert(false && "Unknown logTarget");
                break;
        }
    }

    std::fstream mLogFile;
};

Logger& GetLogger()
{
    static Logger object;
    return object;
}