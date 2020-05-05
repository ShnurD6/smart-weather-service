#pragma once

#include <string>
#include <variant>
#include <boost/format.hpp>
#include "ApiConnection/ApiConnection.hpp"
#include "QueryGenerators/QueryGenerators.hpp"
#include "JsonParser/JsonParser.hpp"
#include "Logger/Logger.hpp"

using namespace std::string_literals;
using json = nlohmann::json;

class WeatherCore
{
    ApiConnection mWeatherApi{"api.openweathermap.org", "/data/2.5/onecall"};
    ApiConnection mCitiesApi{"api.opencagedata.com", "/geocode/v1/json"};

    using TError = std::string;

    using TCoordinates = std::pair<double, double>;

public:

    struct Reply
    {
        using TRequestId = int64_t;

        TRequestId RequestId{-1};

        TError Error;

        std::vector<std::string> PossibleCityNames;

        Reply& SetError(TError&& aError)
        {
            Error = std::forward<TError>(aError);
            return *this;
        }
    };

    std::string GetWeatherByLocation(
        double aLatitude,
        double aLongitude) const
    {
        return GetWeatherByLocation(
            std::to_string(aLatitude),
            std::to_string(aLongitude));
    }

    std::string GetWeatherByLocation(
        const std::string& aLatitude,
        const std::string& aLongitude) const
    {
        Log((boost::format("GetWeatherByLocation(%s, %s)") % aLatitude % aLongitude).str());

        std::stringstream result;

        if (!isCorrectCoordinate(aLatitude) || !isCorrectCoordinate(aLongitude))
            return "Coordinates are incorrect. Please, enter correct coordinate";

        const auto weatherQuery = WeatherQueryGenerator()
            .SetLatitude(aLatitude)
            .SetLongitude(aLongitude)
            .MakeQuery();
        
        const auto parseResult = SafetyParseJson(mWeatherApi.MakeRequest(weatherQuery));

        if (std::holds_alternative<TError>(parseResult))
            return std::get<TError>(parseResult);

        const auto& response = std::get<json>(parseResult);

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

    Reply RegisterRequestByCityName(const std::string& aCityName) const
    {
        Reply result;

        const auto citiesQuery = CitiesQueryGenerator()
            .SetCityName(aCityName)
            .MakeQuery();

        auto parseResult = SafetyParseJson(mCitiesApi.MakeRequest(citiesQuery));

        if (std::holds_alternative<TError>(parseResult))
            return result.SetError(std::move(std::get<TError>(parseResult)));

        const auto& response = std::get<json>(parseResult);

        if (auto outErrorMessage = CheckErrorInCitiesApiResponse(response);
            !outErrorMessage.empty())
            return result.SetError(std::move(outErrorMessage));

        result.RequestId = GetNewRequestId();
        result.PossibleCityNames = GetAndRegisterCityNames(response, result.RequestId);

        return result;
    }

    std::string SpecifyCity(Reply::TRequestId aRequestId, size_t aCityNum) const
    {
        const auto& it = mWaitingRequests.find(aRequestId);
        if (it == mWaitingRequests.cend())
            return "Incorrect request id";

        if (aCityNum + 1 < aCityNum || aCityNum + 1 > it->second.size())
            return "Incorrect city choisen. Please, try again.";

        const auto [latitude, longitude] = it->second.at(aCityNum);

        return GetWeatherByLocation(latitude, longitude);
    }

private:

    void CreateCurrentTemperaturePartOfAnswer(const json& aCurrentWeather, std::ostream& aStream) const
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

    void CreateDescriptionPartOfAnswer(const json& aCurrentWeather, std::ostream& aStream) const
    {
        // API always sends an array of one element. Strange, but ok)
        const std::string& weatherDescription = aCurrentWeather["weather"][0]["description"];

        aStream << "На улице " << weatherDescription << "," << std::endl;
    }

    void CreateHumidityPartOfAnswer(const json& aCurrentWeather, std::ostream& aStream) const
    {
        const auto& weatherHumidity = aCurrentWeather["humidity"];

        aStream << "влажность воздуха составляет " << weatherHumidity << "%," << std::endl;
    }

    void CreateWindSpeedPartOfAnswer(const json& aCurrentWeather, std::ostream& aStream) const
    {
        const auto& windSpeed = aCurrentWeather["wind_speed"];

        aStream << "скорость ветра " << windSpeed << " м/c" << std::endl;
    }

    std::string CheckErrorInWeatherApiResponse(const json& aResponse) const
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

    std::string CheckErrorInCitiesApiResponse(const json& aResponse) const
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

    bool isCorrectCoordinate(const std::string& aPotentialCoordinate) const
    {
        if (aPotentialCoordinate.empty())
            return false;

        for (char c: aPotentialCoordinate)
            if (!std::isdigit(c) && c != '.' && c != '-')
                return false;

        return true;
    }

    // Process JSON and return parsed responce or error
    auto SafetyParseJson(std::string&& aResponse) const
        -> std::variant<json, TError>
    {
        try
        {
            return json::parse(aResponse);
        }
        catch (json::exception& aException)
        {
            Log("Catch parse exception: "s + aException.what());

            return "Something wrong with you request, please, check all and resend.\n"
                   "Notice: We can parse only latin letters"s;
        }
    }

    auto GetAndRegisterCityNames(const json& aResponse, Reply::TRequestId aRequestId) const
        -> std::vector<std::string>
    {
        std::vector<std::string> possibleCities;

        auto& req = mWaitingRequests[aRequestId];
        for (const json& resultIt: aResponse["results"])
        {
            possibleCities.emplace_back(resultIt["formatted"]);
            const auto& coordinates = resultIt["geometry"];

            req.emplace_back(TCoordinates{coordinates["lat"], coordinates["lng"]});
        }

        return possibleCities;
    }

    Reply::TRequestId GetNewRequestId() const
    {
        return mRequestId++;
    }

    void Log(const std::string&& aNewLog) const
    {
        GetLogger().Log("[Core] " + aNewLog);
    }

    mutable Reply::TRequestId mRequestId;
    mutable std::map<Reply::TRequestId, std::vector<TCoordinates>> mWaitingRequests;
};