#include "WeatherCore/WeatherCore.hpp"
#include "ConsoleClient.hpp"

int main()
{
    WeatherCore core;
    ConsoleClient::Start(core);

    return 0;
}