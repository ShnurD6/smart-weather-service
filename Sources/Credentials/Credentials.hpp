#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

class Credentials
{
    static constexpr auto PathToWeatherToken = "WeatherToken.api";
    static constexpr auto PathToCitiesToken = "CitiesToken.api";
    static constexpr auto PathToTelegramToken = "TelegramToken.api";

public:
    std::string& GetWeatherToken()
    {
        static std::string weatherToken{
            ReadTokenFromFile(PathToWeatherToken)};

        return weatherToken;
    }

    std::string& GetCitiesToken()
    {
        static std::string citiesToken{
            ReadTokenFromFile(PathToCitiesToken)};

        return citiesToken;
    }

    std::string& GetTelegramToken()
    {
        static std::string telegramToken{
            ReadTokenFromFile(PathToTelegramToken)};

        return telegramToken;
    }

private:

    std::string ReadTokenFromFile(const char* aPath)
    {
        std::ifstream file(aPath);

        if (!file)
        {
            std::cout << "Cannot open a file with token :(\n"
                         "Please, add " << aPath << " file to project root" << std::endl;
            exit(-1);
        }

        auto result = std::string(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>());

        if (result.empty())
        {
            std::cout << "File with token " << aPath << " empty :(\n"
                         "Please, write correct token to the file." << std::endl;
            exit(-1);
        }

        return result;
    }
};

Credentials& GetCredentials()
{
    static Credentials object;
    return object;
}


