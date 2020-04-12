#include <iostream>
#include "ApiConnection/ApiConnection.hpp"
#include "Credentials/Credentials.hpp"
#include "QueryGenerators/QueryGenerators.hpp"
#include "WeatherCore/WeatherCore.hpp"

using json = nlohmann::json;
using namespace boost::beast::http;

int main()
{
    WeatherCore core;

    std::string Latitude = "60.99";
    std::string Longitude = "30.9";

    std::cout << std::endl << core.GetWeatherByLocation(Latitude, Longitude) << std::endl;

    return 0;
}