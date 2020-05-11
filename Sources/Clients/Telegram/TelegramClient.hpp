#pragma once

#include <tgbot/tgbot.h>
#include <string>
#include "Logger/Logger.hpp"

#include "WeatherCore/WeatherCore.hpp"

struct TelegramClient
{
    TelegramClient(WeatherCore& aWeatherCore)
        : Bot(GetCredentials().GetTelegramToken())
        , Core(aWeatherCore)
    {
        Log("Initialized", Logger::LogType::Debug);
        Bot.getEvents().onCommand(
            "start",
            [this](const TgBot::Message::Ptr& aMessage)
            {
                this->ProcessStartCommand(aMessage);
            });

        Bot.getEvents().onCallbackQuery(
            [this](const TgBot::CallbackQuery::Ptr& aQuery)
            {
                this->ProcessQuery(aQuery);
            });

        Bot.getEvents().onAnyMessage(
            [this](const TgBot::Message::Ptr& aMessage)
            {
                this->AllMessagesHandler(aMessage);
            });
    }

    void Start()
    {
        try
        {
            std::cout << "Bot username: " << Bot.getApi().getMe()->username << std::endl;
            Bot.getApi().deleteWebhook();
            TgBot::TgLongPoll longPoll(Bot);

            Log("Started with username " + Bot.getApi().getMe()->username, Logger::LogType::Debug);
            while (true)
            {
                std::cout << "Long poll started" << std::endl;
                longPoll.start();
            }
        }
        catch (TgBot::TgException& e)
        {
            std::cerr << "TgException: " << e.what() << std::endl;
        }
    }

private:

    TgBot::Bot Bot;
    WeatherCore& Core;

    void AllMessagesHandler(const TgBot::Message::Ptr& aMessage) const
    {
        Log((boost::format("User %s (%d) send message '%s'")
            % aMessage->from->username
            % aMessage->from->id
            % aMessage->text).str(), Logger::LogType::Debug);

        std::string answer;

        if (StringTools::startsWith(aMessage->text, "/"))
        {
            if (aMessage->text != "/start")
                SimpleSendMessage(
                    aMessage->from->id,
                    "Unknown command :(",
                    aMessage->messageId);

            return;
        }

        if (aMessage->location)
            GetWeatherByLocation(aMessage);
        else
            GetWeatherByCityName(aMessage);
    }

    // Get string format "RequestId&CityNum&MessageId" and parse it
    auto GetIdsFromQuery([[maybe_unused]] const std::string& aQuery)
        -> std::tuple<WeatherCore::Reply::TRequestId, size_t, std::int32_t>
    {
        std::vector<std::string> parseRes;

        boost::split(parseRes, aQuery, boost::is_any_of("&"));

        if (parseRes.size() == 2)
        {
            Log("Catch old query format. Set zero to MessageId.", Logger::LogType::Debug);
            parseRes.emplace_back("0");
        }
        assert(parseRes.size() == 3);

        Log((boost::format("Parsed query: RequestId = %s, CityNum = %s, MessageId = %s")
            % parseRes[0]
            % parseRes[1]
            % parseRes[2]).str(), Logger::LogType::Debug);

        return {std::stoll(parseRes[0]), std::stoul(parseRes[1]), std::stoll(parseRes[2])};
    }

    void SimpleSendMessage(std::int64_t aChatId, const std::string& aMessage, std::int32_t replyToMessageId = 0) const
    {
        Log("Send \'" + aMessage + "\' to chat " + std::to_string(aChatId) + "...", Logger::LogType::Debug);

        Bot.getApi().sendMessage(
            aChatId,
            aMessage,
            false, // disableWebPagePreview
            replyToMessageId);

        Log("[" + std::to_string(aChatId) + "] Done", Logger::LogType::Debug);
    }

    void ProcessStartCommand(const TgBot::Message::Ptr& aMessage) const
    {
        SimpleSendMessage(
            aMessage->chat->id,
            "Привет!\n"
            "Я бот, который позволяет узнать погоду :)\n"
            "Скинь мне локацию или название города на латинице",
            aMessage->messageId);
    }

    void ProcessQuery(const TgBot::CallbackQuery::Ptr& aQuery)
    {
        Log((boost::format("User %s (%d) send query: '%s'")
            % aQuery->from->username
            % aQuery->from->id
            % aQuery->data).str(), Logger::LogType::Debug);

        const auto [requestId, cityId, messageId] = GetIdsFromQuery(aQuery->data);

        SimpleSendMessage(
            aQuery->from->id,
            Core.SpecifyCity(requestId, cityId),
            messageId);
    }

    void GetWeatherByLocation(const TgBot::Message::Ptr& aMessage) const
    {
        auto latitude = aMessage->location->latitude;
        auto longitude = aMessage->location->longitude;

        Log((boost::format("parse location: {latitude = %f, longitude = %f}")
            % latitude
            % longitude).str(), Logger::LogType::Debug);

        SimpleSendMessage(
            aMessage->from->id,
            Core.GetWeatherByLocation(latitude, longitude),
            aMessage->messageId);
    }

    void GetWeatherByCityName(const TgBot::Message::Ptr& aMessage) const
    {
        auto reply = Core.RegisterRequestByCityName(aMessage->text);

        if (!reply.Error.empty())
        {
            SimpleSendMessage(
                aMessage->from->id,
                reply.Error,
                aMessage->messageId);

            return;
        }

        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
        size_t cityNum = 0;

        for (const auto& city: reply.PossibleCityNames)
        {
            TgBot::InlineKeyboardButton::Ptr cityButton(new TgBot::InlineKeyboardButton);
            cityButton->text = city;
            cityButton->callbackData = (boost::format("%d&%d&%d")
                % reply.RequestId
                % cityNum
                % aMessage->messageId).str();

            keyboard->inlineKeyboard.push_back({{cityButton}});
        }

        Bot.getApi().sendMessage(
            aMessage->chat->id,
            "Уточните город:",
            false, // disableWebPagePreview
            aMessage->messageId,      // replyToMessageId
            keyboard);
    }

    template <typename T>
    void Log(T&& aNewLog, Logger::LogType aLogType) const
    {
        GetLogger().Log("[TgCli] "s + std::forward<T>(aNewLog), aLogType);
    }
};
