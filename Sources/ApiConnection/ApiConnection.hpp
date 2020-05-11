#pragma once

#include <iostream>
#include "JsonParser/JsonParser.hpp"
#include "Logger/Logger.hpp"
#include <cpr/cpr.h>

using namespace std::string_literals;
class ApiConnection
{
public:

    ApiConnection(
        const std::string& aHost,
        const std::string& aApiTarget)
        : mRequestUrl(aHost + aApiTarget)
    {};

    [[nodiscard]] std::string MakeRequest(const std::string& aQueryString) const
    {
        std::string urlWithQuery{mRequestUrl + aQueryString};

        Log("Send " + aQueryString + " to " + mRequestUrl + "...", Logger::LogType::Debug);

        auto r = cpr::Get(cpr::Url{urlWithQuery});

        return r.text;
    }

private:

    std::string mRequestUrl;

    template <typename T>
    void Log(T&& aNewLog, Logger::LogType aLogType) const
    {
        GetLogger().Log("[ApiCon] " + std::forward<T>(aNewLog), aLogType);
    }
};
