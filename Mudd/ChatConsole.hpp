#pragma once

#include "CommonConst.hpp"
#include "Console.hpp"

#include <string>
#include <vector>

struct ChatData
{
    public:
    ChatData() :
        room(GLOBAL), username(), chatMessage()
    {}

    ChatData(RoomType room, std::string username, std::string chatMessage) :
        room(room), username(username), chatMessage(chatMessage)
    {}

    RoomType room;
    std::string username;
    std::string chatMessage;
};

class ChatConsole
{
    public:

    ChatConsole() :
        _consoleLine(0), _offsetX(0), _offsetY(0), _width(0), _height(0), _messages(), _console() {}

    ChatConsole(int x, int y, int width, int height) :
        _consoleLine(0), _offsetX(x), _offsetY(y), _width(width), _height(height), _messages(), _console() {}

    void AddMessage(RoomType room, const std::string& username, const std::string& chatMessage);

    void UpdateDisplay(bool fullRefresh);

    void RefreshDisplay(void);

    void DisplayMessage(const ChatData& message, bool progressive);

    bool ShouldDisplay(const ChatData& message);

    std::string FormatChatMessage(const ChatData& message);

    std::string GetRoomName(RoomType room);

    private:
    int _consoleLine;
    int _offsetX;
    int _offsetY;
    int _width;
    int _height;
    std::vector<ChatData> _messages;
    Console _console;

};
