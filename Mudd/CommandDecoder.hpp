#pragma once

#include <memory>
#include <string.h>

#include "Message.hpp"

class CommandDecoder
{
    public:
    CommandDecoder() : _userId(0) {}
    CommandDecoder(int userId) : _userId(userId) {}

    std::unique_ptr<Message> Decode(const std::string& command);

    private:
    int _userId;
};