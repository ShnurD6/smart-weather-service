#include <iostream>
#include "ApiConnection/ApiConnection.hpp"
#include "Credentials/Credentials.hpp"
#include "QueryGenerators/WeatherQueryGenerator.hpp"

namespace http = boost::beast::http;

int main()
{
    ApiConnection WeatherApi("api.openweathermap.org", "/data/2.5/onecall");

    const auto q = WeatherQueryGenerator()
        .SetLatitude("60.99")
        .SetLongitude("30.9")
        .MakeQuery();

    std::cout << WeatherApi.MakeRequest(q);
    return 0;
}