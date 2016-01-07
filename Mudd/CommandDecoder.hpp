#pragma once

#include <memory>
#include <string.h>

#include "Message.hpp"

class CommandDecoder
{
    public:
    static std::unique_ptr<Message> Decode(const std::string& command);
};