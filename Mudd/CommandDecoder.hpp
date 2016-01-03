#pragma once

#include <memory>
#include <string.h>
#include "Message.hpp"

class CommandDecoder
{
    public:
    static std::unique_ptr<Message> Decode(const std::string& command)
    {
        if (command.find("/gettime") == 0)
        {
            return std::unique_ptr<Message>(new TimeMessage());
        }
        else
            return std::unique_ptr<Message>(new ChatMessage(command));
    }
};