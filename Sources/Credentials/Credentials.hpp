#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

class Credentials
{
    static constexpr auto PathToWeatherToken = "WeatherToken.api";
    static constexpr auto WeatherTokenEnv = "SWS_WEATHER_TOKEN";

    static constexpr auto PathToCitiesToken = "CitiesToken.api";
    static constexpr auto CitiesTokenEnv = "SWS_CITIES_TOKEN";

    static constexpr auto PathToTelegramToken = "TelegramToken.api";
    static constexpr auto TelegramTokenEnv = "SWS_TELEGRAM_TOKEN";

public:
    std::string& GetWeatherToken()
    {
        static std::string weatherToken{
            GetToken(
                "OpenWeatherMap",
                PathToWeatherToken,
                WeatherTokenEnv)};

        return weatherToken;
    }

    std::string& GetCitiesToken()
    {
        static std::string citiesToken{
            GetToken(
                "OpenCageData",
                PathToCitiesToken,
                CitiesTokenEnv)};

        return citiesToken;
    }

    std::string& GetTelegramToken()
    {
        static std::string telegramToken{
            GetToken(
                "Telegram",
                PathToTelegramToken,
                TelegramTokenEnv)};

        return telegramToken;
    }

private:

    std::string GetToken(
        const char* aTokenName,
        const char* aPathToFile,
        const char* aEnvVar)
    {
        std::string token;

        token = GetTokenFromEnv(aEnvVar);
        if (!token.empty())
            return token;

        token = GetTokenFromFile(aPathToFile);
        if (!token.empty())
            return token;

        std::cerr << "Cannot find a " << aTokenName << " token.\n"
                     "Please, add " << aPathToFile << " file to project root or " << aEnvVar << " env variable" << std::endl;
        exit(-1);
    }

    std::string GetTokenFromEnv(const char* aEnvVar)
    {
        const auto envToken = std::getenv(aEnvVar);

        return envToken
            ? envToken
            : "";
    }

    std::string GetTokenFromFile(const char* aPathToFile)
    {
        std::string result;
        std::ifstream file(aPathToFile);

        if (!file)
            return result;

        result = std::string(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>());

        return result;
    }
};

Credentials& GetCredentials()
{
    static Credentials object;
    return object;
}


