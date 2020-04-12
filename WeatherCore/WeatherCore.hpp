#pragma once
#include <string>
#include "../ApiConnection/ApiConnection.hpp"
#include "../QueryGenerators/QueryGenerators.hpp"
#include "../JsonParser/JsonParser.hpp"

using json = nlohmann::json;

class WeatherCore
{
    ApiConnection mWeatherApi{"api.openweathermap.org", "/data/2.5/onecall"};
    ApiConnection mCitiesApi{"api.opencagedata.com", "/geocode/v1/json"};

public:

    std::string GetWeatherByLocation(std::string& aLatitude, std::string& aLongitude)
    {
        std::stringstream result;

        const auto weatherQuery = WeatherQueryGenerator()
            .SetLatitude(aLatitude)
            .SetLongitude(aLongitude)
            .MakeQuery();

        const auto response = json::parse(mWeatherApi.MakeRequest(weatherQuery));

        if (auto error = CheckErrorInWeatherApiResponse(response); !error.empty())
            return error;

        const auto& currentWeather = response["current"];

        result << "Ваш прогноз погоды ☄:" << std::endl;

        // Сейчас темпетатура составляет ... (и ощущается как ... / по ощущениям так же)
        CreateCurrentTemperaturePartOfAnswer(currentWeather, result);

        return result.str();
    }

private:

    void CreateCurrentTemperaturePartOfAnswer(const json& aCurrentWeather, std::ostream& aStream)
    {
        int actualTemp = aCurrentWeather["dew_point"];
        int feelsLike = aCurrentWeather["feels_like"];

        aStream << "Сейчас температура составляет " << actualTemp << "℃, ";

        if (actualTemp != feelsLike)
            aStream << "но ощущается как " << feelsLike;
        else
            aStream << "по ощущениям так же :)";

        aStream << "℃" << std::endl;
    }

    std::string CheckErrorInWeatherApiResponse(const json& aResponse)
    {
        return {};
    }
};