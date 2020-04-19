#pragma once
#include <string>
#include "ApiConnection/ApiConnection.hpp"
#include "QueryGenerators/QueryGenerators.hpp"
#include "JsonParser/JsonParser.hpp"

using json = nlohmann::json;

class WeatherCore
{
    ApiConnection mWeatherApi{"api.openweathermap.org", "/data/2.5/onecall"};
    ApiConnection mCitiesApi{"api.opencagedata.com", "/geocode/v1/json"};

public:

    std::string GetWeatherByLocation(
        const std::string& aLatitude,
        const std::string& aLongitude)
    {
        std::stringstream result;

        if (!isCorrectCoordinate(aLatitude) || !isCorrectCoordinate(aLongitude))
            return "Coordinates are incorrect. Please, enter correct coordinate";

        const auto weatherQuery = WeatherQueryGenerator()
            .SetLatitude(aLatitude)
            .SetLongitude(aLongitude)
            .MakeQuery();
        
        const auto [response, parseError] = SafetyParseJson(mWeatherApi.MakeRequest(weatherQuery));

        if (!parseError.empty())
            return parseError;
        if (auto errorInResponse = CheckErrorInWeatherApiResponse(response); !errorInResponse.empty())
            return errorInResponse;

        const auto& currentWeather = response["current"];

        result << "Ваш прогноз погоды ☄:" << std::endl;

        // Сейчас темпетатура составляет ... (и ощущается как ... / по ощущениям так же)
        CreateCurrentTemperaturePartOfAnswer(currentWeather, result);

        // На улице пасмурно/солнечно/etc
        CreateDescriptionPartOfAnswer(currentWeather, result);

        // Влажность возжуха составляет X%
        CreateHumidityPartOfAnswer(currentWeather, result);

        // Скорость ветра n м/c
        CreateWindSpeedPartOfAnswer(currentWeather, result);

        return result.str();
    }

    std::string GetWeatherByCityName(
        const std::string& aCityName,
        const std::function<size_t(std::vector<std::string>&)>& aChoiseCitiesCallback)
    {
        const auto citiesQuery = CitiesQueryGenerator()
            .SetCityName(aCityName)
            .MakeQuery();

        const auto [response, parseError] = SafetyParseJson(mCitiesApi.MakeRequest(citiesQuery));

        if (!parseError.empty())
            return parseError;
        if (auto error = CheckErrorInCitiesApiResponse(response); !error.empty())
            return error;

        const auto& [city, choiseCityError] = ChoiseCity(response, aChoiseCitiesCallback);
        if (!choiseCityError.empty())
            return choiseCityError;

        const auto& coordinates = city["geometry"];

        double latitude = coordinates["lat"];
        double longitude = coordinates["lng"];

        auto latitudeStr = std::to_string(latitude);
        auto longitudeStr = std::to_string(longitude);

        return (GetWeatherByLocation(latitudeStr, longitudeStr));
    }

private:

    void CreateCurrentTemperaturePartOfAnswer(const json& aCurrentWeather, std::ostream& aStream)
    {
        int actualTemp = aCurrentWeather["temp"];
        int feelsLike = aCurrentWeather["feels_like"];

        aStream << "Сейчас температура составляет " << actualTemp << "℃, ";

        if (actualTemp != feelsLike)
            aStream << "но ощущается как " << feelsLike << "℃";
        else
            aStream << "по ощущениям так же";

        aStream << " :)" << std::endl;
    }

    void CreateDescriptionPartOfAnswer(const json& aCurrentWeather, std::ostream& aStream)
    {
        // API always sends an array of one element. Strange, but ok)
        const std::string& weatherDescription = aCurrentWeather["weather"][0]["description"];

        aStream << "На улице " << weatherDescription << "," << std::endl;
    }

    void CreateHumidityPartOfAnswer(const json& aCurrentWeather, std::ostream& aStream)
    {
        const auto& weatherHumidity = aCurrentWeather["humidity"];

        aStream << "влажность воздуха составляет " << weatherHumidity << "%," << std::endl;
    }

    void CreateWindSpeedPartOfAnswer(const json& aCurrentWeather, std::ostream& aStream)
    {
        const auto& windSpeed = aCurrentWeather["wind_speed"];

        aStream << "скорость ветра " << windSpeed << " м/c" << std::endl;
    }

    std::string CheckErrorInWeatherApiResponse(const json& aResponse)
    {
        unsigned responseCode;

        if (aResponse.contains("cod"))
        {
            // This is a VERY weird API.
            // Sometimes it sends the code with a number, BUT sometimes with a string ))
            if (aResponse["cod"].is_string())
                responseCode = std::stoi(aResponse["cod"].get<std::string>());
            else
                responseCode = aResponse["cod"].get<unsigned>();

            switch (responseCode)
            {
                case 400:
                    return "Incorrect coordinates. Please, try again";
                case 401:
                    return "Invalid Weather token :(";
                case 429:
                    return "Weather token exceeding of requests limitation of your subscription type.\n"
                           "Refresh the token, please :(";
                case 503:
                    return "Weather API is down :(\n"
                           "Try again latter";
            }
        }

        return {};
    }

    std::string CheckErrorInCitiesApiResponse(const json& aResponse)
    {
        unsigned responseCode = aResponse["status"]["code"];

        switch (responseCode)
        {
            case 401:
                return "Invalid Cities token :(";
            case 402:
                return "Sorry, but you exceeded cities token (payment required) :(";
            case 408:
                return "Timeout in Cities API. Try again)";
            case 429:
                return "Too many requests to Cities API. Try again)";
            case 503:
                return "Cities API is down :(\n"
                       "Try again latter";
        }

        if (aResponse["results"].empty())
            return "Please, enter the correct city.";

        return {};
    }
    
    bool isCorrectCoordinate(const std::string& aPotentialCoorginate)
    {
        if (aPotentialCoorginate.empty())
            return false;

        for (char c: aPotentialCoorginate)
            if (!std::isdigit(c) && c != '.' && c != '-')
                return false;

        return true;
    }
    
    auto SafetyParseJson(std::string&& aResponse)
//              <response, error>
        -> std::pair<json, std::string>
    {

        try
        {
            return {json::parse(aResponse), {}};
        }
        catch (json::exception&)
        {
            return {
            json{},
            "Something wrong with you request, please, check all and resend.\n"
               "Notice: We can parse only latin letters"};
        }
    }

    auto ChoiseCity(
        const json& aResponse,
        const std::function<size_t(std::vector<std::string>&)>& aChoiseCitiesCallback)
//                  result, error
        -> std::pair<json, std::string>
    {
        std::vector<std::string> possibleCities;

        for (const json& resultIt: aResponse["results"])
            possibleCities.emplace_back(resultIt["formatted"]);

        size_t cityNum = aChoiseCitiesCallback(possibleCities);

        if (cityNum + 1 < cityNum || cityNum + 1 > aResponse["results"].size())
            return {json{}, "Incorrect city choisen. Please, try again."};

        return {aResponse["results"][cityNum], {}};
    }
};