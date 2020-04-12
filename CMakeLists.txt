cmake_minimum_required(VERSION 3.16)
project(SmartWeatherService)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

set(SOURCES
        main.cpp
        ApiConnection/ApiConnection.hpp
        Credentials/Credentials.hpp
        QueryGenerators/WeatherQueryGenerator.hpp
        QueryGenerators/CitiesQueryGenerator.hpp
        JsonParser/JsonParser.hpp "WeatherCore/WeatherCore.hpp")

add_executable(SmartWeatherService ${SOURCES})
target_link_libraries(SmartWeatherService pthread )