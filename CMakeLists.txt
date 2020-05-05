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
        Sources/WeatherCore/WeatherCore.hpp
        Sources/Clients/ConsoleClient.hpp Sources/Clients/TelegramClient.hpp Sources/Logger/Logger.hpp)

include_directories(Sources /usr/local/include)

add_executable(SmartWeatherService ${SOURCES})

target_link_libraries(SmartWeatherService pthread ssl crypto /usr/local/lib/libTgBot.a)
