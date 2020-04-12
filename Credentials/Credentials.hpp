#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

class Credentials
{
    static constexpr auto PathToWeatherTocken = "WeatherTocken.api";
    static constexpr auto PathToCitiesTocken = "CitiesTocken.api";

public:
    std::string& GetWeatherTocken()
    {
        static std::string weatherTocken{
            ReadTockenFromFile(PathToWeatherTocken)};

        return weatherTocken;
    }

    std::string& GetCitiesTocken()
    {
        static std::string citiesTocken{
            PathToCitiesTocken};

        return citiesTocken;
    }

private:

    std::string ReadTockenFromFile(const char* aPath)
    {
        std::ifstream file(aPath);

        if (!file)
        {
            std::cout << "Cannot open a file with tocken :(\n"
                         "Please, add " << aPath << " file" << std::endl;
            exit(-1);
        }

        auto result = std::string(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>());

        if (result.empty())
        {
            std::cout << "File with tocken " << aPath << " empty :(\n"
                         "Please, write correct tocken to the file." << std::endl;
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


