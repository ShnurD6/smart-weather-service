#pragma once
#include <string>
#include <fstream>
#include <boost/date_time.hpp>

struct Logger
{
    void Log(const std::string& aNewLog)
    {
        mLogFile << boost::posix_time::second_clock::local_time() << ": " << aNewLog << std::endl;
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

    std::fstream mLogFile;
};

Logger& GetLogger()
{
    static Logger object;
    return object;
}