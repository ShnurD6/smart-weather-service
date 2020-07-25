cmake_minimum_required(VERSION 3.16)
project(SmartWeatherService)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    Sources/ApiConnection/ApiConnection.hpp
    Sources/Credentials/Credentials.hpp
    Sources/QueryGenerators/WeatherQueryGenerator.hpp
    Sources/QueryGenerators/CitiesQueryGenerator.hpp
    Sources/WeatherCore/WeatherCore.hpp
    Sources/Clients/Console/ConsoleClient.hpp
    Sources/Clients/Telegram/TelegramClient.hpp)

add_subdirectory(Sources/SimpleLoggerLib)

include_directories(Sources Sources/JsonParser/single_include)

add_executable(ConsoleClient ${SOURCES} ${LOGGER_SOURCES} Sources/Clients/Console/main.cpp)
target_link_libraries(ConsoleClient pthread cpr curl Logger)

add_executable(TelegramClient ${SOURCES} ${LOGGER_SOURCES} Sources/Clients/Telegram/main.cpp)
target_link_libraries(TelegramClient pthread ssl crypto cpr curl /usr/local/lib/libTgBot.a Logger)