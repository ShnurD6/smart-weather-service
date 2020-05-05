#pragma once
#include "WeatherCore/WeatherCore.hpp"

struct ConsoleClient
{
    static size_t ChosieCity(const std::vector<std::string> &aPossibleCities)
    {
        size_t result = 1; // <-------------------------------------------------\
                                                                            //  |
        std::cout << "Specify the city:" << std::endl;                      //  |
        for (const auto &possibleCity : aPossibleCities)                    //  |
        {                                                                   //  |
            std::cout << result++ << ": " << possibleCity << std::endl;     //  |
        }                                                                   //  |
        std::cin >> result;                                                 //  |
        std::cout << std::endl;                                             //  |
        return --result; // <-- This is only for the convenience of the user) <-/
    }

    static void Start(const WeatherCore& aCore)
    {
        short buf;

        std::cout << "Enter mode:\n"
                     "[0] - by coordinates\n"
                     "[1] - by city name" << std::endl;
        std::cin >> buf;

        if (buf == 0)
        {
            std::string Latitude;
            std::string Longitude;

            std::cout << "Enter Latitude:" << std::endl;
            std::cin >> Latitude;

            std::cout << "Enter Longitude:" << std::endl;
            std::cin >> Longitude;

            std::cout << "\n" << aCore.GetWeatherByLocation(Latitude, Longitude) << std::endl;
        }
        else if (buf == 1)
        {
            std::string city_name;

            std::cout << "Enter city name:" << std::endl;
            std::cin >> city_name;

            auto reqReply = aCore.RegisterRequestByCityName(city_name);

            if (!reqReply.Error.empty())
                std::cout << reqReply.Error;

            std::cout << aCore.SpecifyCity(
                reqReply.RequestId,
                ChosieCity(reqReply.PossibleCityNames));
        }
    }

};