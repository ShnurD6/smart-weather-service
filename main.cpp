#include <iostream>
#include "ApiConnection/ApiConnection.hpp"
#include "Credentials/Credentials.hpp"

namespace http = boost::beast::http;

int main() {
    ApiConnection WeatherApi("api.openweathermap.org", "/data/2.5/onecall");

    // TODO: Add generator of queries
    std::string q("?lat=60.99&lon=30.9&appid=" + GetCredentials().GetWeatherTocken() + "&lang=ru");

    std::cout << WeatherApi.MakeRequest(q);
    return 0;
}