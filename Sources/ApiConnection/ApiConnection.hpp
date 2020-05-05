#pragma once

#include <iostream>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include "JsonParser/JsonParser.hpp"
#include "Logger/Logger.hpp"

using namespace std::string_literals;
namespace http = boost::beast::http;

class ApiConnection
{
public:

    ApiConnection(
        const char* aHost,
        const char* aApiTarget)
        : mResolver(mIoc)
        , mSocket(mIoc)
        , mHost(aHost)
        , mApiTarget(aApiTarget)
    {
        Log("Connecting to "s + mHost + "..."s);
        boost::asio::connect(mSocket, mResolver.resolve(mHost, "80"));
        Log("Connecting to "s + mHost + " success!"s);
    };

    ~ApiConnection()
    {
        mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    }

    [[nodiscard]] std::string MakeRequest(const std::string& aQueryString) const
    {
        http::request<http::string_body> request(http::verb::get, mApiTarget + aQueryString, 11);
        request.set(http::field::host, mHost);
        request.prepare_payload();

        Log("Send " + aQueryString + " to " + mHost + "...");

        http::write(mSocket, request);

        boost::beast::flat_buffer buffer;

        http::response<http::string_body> result;

        try
        {
            http::read(mSocket, buffer, result);
        }
        catch (std::exception& aException)
        {
            Log("Catch exception when read from socket: "s + aException.what());
        }

        Log("Get result from "s + mHost);

        return result.body();
    }

private:

    boost::asio::io_context mIoc;

    boost::asio::ip::tcp::resolver mResolver;
    mutable boost::asio::ip::tcp::socket mSocket;

    const char* mHost;
    const char* mApiTarget;

    void Log(const std::string&& aNewLog) const
    {
        GetLogger().Log("[ApiCon] " + aNewLog);
    }
};
