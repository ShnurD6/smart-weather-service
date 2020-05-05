#include "WeatherCore/WeatherCore.hpp"
#include "TelegramClient.hpp"

int main()
{
    WeatherCore core;
    TelegramClient client(core);

    client.Start();

    return 0;
}