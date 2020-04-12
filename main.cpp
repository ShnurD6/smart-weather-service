#include <iostream>
#include "ApiConnection/ApiConnection.hpp"
#include "Credentials/Credentials.hpp"
#include "QueryGenerators/QueryGenerators.hpp"

namespace http = boost::beast::http;

int main()
{
    ApiConnection WeatherApi("api.openweathermap.org", "/data/2.5/onecall");

    const auto weatherQuery = WeatherQueryGenerator()
        .SetLatitude("60.99")
        .SetLongitude("30.9")
        .MakeQuery();
    std::cout << "Weather result = " << WeatherApi.MakeRequest(weatherQuery) << std::endl;

    ApiConnection CitiesApi("api.opencagedata.com", "/geocode/v1/json");

    const auto citiesQuery = CitiesQueryGenerator()
        .SetCityName("Moscow")
        .MakeQuery();

    std::cout << "Cities result = " << CitiesApi.MakeRequest("?q=Moscow&key=035799cf1ad64c47bf33ba8c776126ab") << std::endl;
    return 0;
}