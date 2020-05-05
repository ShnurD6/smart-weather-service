#include <iostream>

#include "ApiConnection/ApiConnection.hpp"
#include "WeatherCore/WeatherCore.hpp"

#include "Clients/ConsoleClient.hpp"
#include "Clients/TelegramClient.hpp"

using json = nlohmann::json;
using namespace boost::beast::http;

int main()
{
    WeatherCore core;
    short a;
    std::cout << "1 - Console, 2 - Tg:" << std::endl;
    std::cin >> a;

    switch (a)
    {
        case 1:
            ConsoleClient::Start(core);
            break;
        case 2:
        {
            TelegramClient client(core);
            client.Start();
            break;
        }
        default:
            break;
    }

    return 0;
}