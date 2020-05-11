cmake_minimum_required(VERSION 3.16)
project(SmartWeatherService)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    Sources/ApiConnection/ApiConnection.hpp
    Sources/Credentials/Credentials.hpp
    Sources/QueryGenerators/WeatherQueryGenerator.hpp
    Sources/QueryGenerators/CitiesQueryGenerator.hpp
    Sources/JsonParser/JsonParser.hpp
    Sources/WeatherCore/WeatherCore.hpp
    Sources/Clients/Console/ConsoleClient.hpp
    Sources/Clients/Telegram/TelegramClient.hpp
    Sources/Logger/Logger.hpp)

include_directories(Sources /usr/local/include)

add_executable(ConsoleClient ${SOURCES} Sources/Clients/Console/main.cpp)
target_link_libraries(ConsoleClient pthread boost_date_time cpr curl)

add_executable(TelegramClient ${SOURCES} Sources/Clients/Telegram/main.cpp)
target_link_libraries(TelegramClient pthread ssl crypto boost_date_time cpr curl /usr/local/lib/libTgBot.a)