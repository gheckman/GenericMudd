#include "CommandDecoder.hpp"

std::unique_ptr<Message> CommandDecoder::Decode(const std::string & command)
{
    if (command.find("/gettime") == 0)
    {
        return std::unique_ptr<Message>(new TimeMessage());
    }
    else
        return std::unique_ptr<Message>(new ChatMessage(command));
}
