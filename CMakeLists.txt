cmake_minimum_required(VERSION 3.16)
project(SmartWeatherService)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
        Sources/main.cpp
        Sources/ApiConnection/ApiConnection.hpp
        Sources/Credentials/Credentials.hpp
        Sources/QueryGenerators/WeatherQueryGenerator.hpp
        Sources/QueryGenerators/CitiesQueryGenerator.hpp
        Sources/JsonParser/JsonParser.hpp
        Sources/WeatherCore/WeatherCore.hpp)

include_directories(Sources)

add_executable(SmartWeatherService ${SOURCES})

target_link_libraries(SmartWeatherService pthread)
