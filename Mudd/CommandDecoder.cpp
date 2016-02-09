#include "CommandDecoder.hpp"

#include <chrono>
#include <cstdint>
#include <limits>

std::unique_ptr<Message> CommandDecoder::Decode(const std::string & command)
{
    if (command.find("/gettime") == 0)
    {
        return std::unique_ptr<Message>(new TimeMessage());
    }
    else if (command.find("/ping") == 0)
    {
        uint64_t msSinceEpoch =
            std::chrono::steady_clock::now().time_since_epoch() /
            std::chrono::milliseconds(1);
        uint16_t msSinceEpoch16 = (msSinceEpoch & UINT16_MAX);

        return std::unique_ptr<Message>(new PingMessage(msSinceEpoch16));
    }
    else
        return std::unique_ptr<Message>(new ChatMessage(command));
}
