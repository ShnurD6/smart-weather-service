#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include "SimpleLoggerLib/Logger.hpp"

using namespace std::string_literals;
class ApiConnection
{
public:

    ApiConnection(
        const std::string& aHost,
        const std::string& aApiTarget)
        : mRequestUrl(aHost + aApiTarget)
        , mLogger("ApiCon")
    {};

    [[nodiscard]] std::string MakeRequest(const std::string& aQueryString) const
    {
        std::string urlWithQuery{mRequestUrl + aQueryString};

        mLogger.DebugLog("Send " + aQueryString + " to " + mRequestUrl + "...");

        auto r = cpr::Get(cpr::Url{urlWithQuery});

        return r.text;
    }

private:

    std::string mRequestUrl;
    LoggerInstance mLogger;
};
