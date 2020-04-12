#pragma once

#include <iostream>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>

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
        boost::asio::connect(mSocket, mResolver.resolve(mHost, "80"));
    };

    ~ApiConnection()
    {
        mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    }

    [[nodiscard]] std::string MakeRequest(const std::string& aQueryString)
    {
        std::cout << "aQuery = " << aQueryString << std::endl;
        http::request<http::string_body> request(http::verb::get, mApiTarget + aQueryString, 11);
        request.set(http::field::host, mHost);
        request.prepare_payload();

        http::write(mSocket, request);

        boost::beast::flat_buffer buffer;

        http::response<http::string_body> result;
        http::read(mSocket, buffer, result);

        return result.body();
    }

private:

    boost::asio::io_context mIoc;

    boost::asio::ip::tcp::resolver mResolver;
    boost::asio::ip::tcp::socket mSocket;

    const char* mHost;
    const char* mApiTarget;
};
