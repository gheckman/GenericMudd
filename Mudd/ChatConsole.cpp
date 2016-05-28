#include "ChatConsole.hpp"
#include "CommonConst.hpp"

void ChatConsole::AddMessage(RoomType room, const std::string& username, const std::string& chatMessage)
{
    _messages.emplace_back(room, username.substr(0, USERNAME_MAX), chatMessage.substr(0, CHAT_MAX));
    if (ShouldDisplay(_messages.back()))
        UpdateDisplay(false);
}

void ChatConsole::UpdateDisplay(bool fullRefresh)
{
    if (fullRefresh || _consoleLine >= _height)
        RefreshDisplay();
    else
        DisplayMessage(_messages.back(), true);
}

void ChatConsole::RefreshDisplay(void)
{
    _consoleLine = _height - 1;
    for (auto it = _messages.rbegin(); it != _messages.rend() && _consoleLine >= 0; ++it)
        if (ShouldDisplay(*it))
            DisplayMessage(*it, false);
    _consoleLine = _height;
}

void ChatConsole::DisplayMessage(const ChatData& message, bool progressive)
{
    auto messageString = FormatChatMessage(message);
    _console.WriteString(_offsetX, _consoleLine + _offsetY, _width, ' ');
    _console.WriteString(_offsetX, _consoleLine + _offsetY, messageString);
    _consoleLine += progressive ? 1 : -1;
}

bool ChatConsole::ShouldDisplay(const ChatData& message)
{
    //return _currentRoom == GLOBAL || message.room == _currentRoom;
    return true;
}

std::string ChatConsole::FormatChatMessage(const ChatData& message)
{
    return GetRoomName(message.room) + "] " + message.username + ": " + std::string(USERNAME_MAX - message.username.size(), ' ') + message.chatMessage;
}

std::string ChatConsole::GetRoomName(RoomType room)
{
    std::string rv;
    switch (room)
    {
        case GLOBAL:   rv = "global"; break;
        case TRADE :   rv = "trade "; break;
        case FACTION : rv = "fact  "; break;
        case WHISPER : rv = "whispr"; break;
    }
    return rv;
}
