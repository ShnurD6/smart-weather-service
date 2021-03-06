#pragma once

#include <tgbot/tgbot.h>
#include <string>
#include "Logger.hpp"

#include "WeatherCore/WeatherCore.hpp"

struct TelegramClient
{
    TelegramClient(WeatherCore& aWeatherCore)
        : Bot(GetCredentials().GetTelegramToken())
        , Core(aWeatherCore)
        , mLogger("TgCli")
    {
        mLogger.DebugLog("Initialized");
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

            mLogger.DebugLog("Started with username " + Bot.getApi().getMe()->username);
            while (true)
                longPoll.start();
        }
        catch (TgBot::TgException& e)
        {
            std::cerr << "TgException: " << e.what() << std::endl;
        }
    }

private:

    TgBot::Bot Bot;
    WeatherCore& Core;
    LoggerInstance mLogger;

    void AllMessagesHandler(const TgBot::Message::Ptr& aMessage) const
    {
        mLogger.DebugLog((boost::format("User %s (%d) send message '%s'")
            % aMessage->from->username
            % aMessage->from->id
            % aMessage->text).str());

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
            mLogger.DebugLog("Catch old query format. Set zero to MessageId.");
            parseRes.emplace_back("0");
        }
        assert(parseRes.size() == 3);

        mLogger.DebugLog((boost::format("Parsed query: RequestId = %s, CityNum = %s, MessageId = %s")
            % parseRes[0]
            % parseRes[1]
            % parseRes[2]).str());

        return {std::stoll(parseRes[0]), std::stoul(parseRes[1]), std::stoll(parseRes[2])};
    }

    void SimpleSendMessage(std::int64_t aChatId, const std::string& aMessage, std::int32_t replyToMessageId = 0) const
    {
        mLogger.DebugLog("Send \'" + aMessage + "\' to chat " + std::to_string(aChatId) + "...");

        Bot.getApi().sendMessage(
            aChatId,
            aMessage,
            false, // disableWebPagePreview
            replyToMessageId);

        mLogger.DebugLog("[" + std::to_string(aChatId) + "] Done");
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
        mLogger.DebugLog((boost::format("User %s (%d) send query: '%s'")
            % aQuery->from->username
            % aQuery->from->id
            % aQuery->data).str());

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

        mLogger.DebugLog((boost::format("parse location: {latitude = %f, longitude = %f}")
            % latitude
            % longitude).str());

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
};
