#pragma once
#include <string>
#include "../Credentials/Credentials.hpp"

class WeatherQueryGenerator
{
    std::string mLatitude;
    std::string mLongitude;
    std::string mLang = "ru";
    std::string mUnits = "metric";

    std::string mTocken = GetCredentials().GetWeatherToken();

public:

    WeatherQueryGenerator& SetLatitude(std::string& aLatitude)
    {
        mLatitude = aLatitude;
        return *this;
    }

    WeatherQueryGenerator& SetLongitude(std::string& aLongitude)
    {
        mLongitude = aLongitude;
        return *this;
    }

    WeatherQueryGenerator& SetLang(std::string& aLang)
    {
        mLang = aLang;
        return *this;
    }

    WeatherQueryGenerator& SetUnits(std::string& aUnits)
    {
        mUnits = aUnits;
        return *this;
    }

    std::string MakeQuery()
    {
        return std::string("?")
            + "lat=" + mLatitude + "&"
            + "lon=" + mLongitude + "&"
            + "appid=" + mTocken + "&"
            + "lang=" + mLang + "&"
            + "units=" + mUnits;
    }
};