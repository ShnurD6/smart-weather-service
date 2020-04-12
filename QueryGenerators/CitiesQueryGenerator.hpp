#pragma once
#include <string>
#include "../Credentials/Credentials.hpp"

class CitiesQueryGenerator
{
    std::string mCityName;

    std::string mTocken = GetCredentials().GetCitiesTocken();

public:

    CitiesQueryGenerator& SetCityName(std::string&& aCityName)
    {
        mCityName = aCityName;
        return *this;
    }

    std::string MakeQuery()
    {
        return std::string("?")
            + "q=" + mCityName + "&"
            + "key=" + mTocken;
    }
};