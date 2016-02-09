#include "Message.hpp"
#include <cstring>

// Message Headers -------------------------------------------------------------

const std::string Message::HEADER = "AMSG";
const std::string VoidMessage::HEADER = "VOID";
const std::string ChatMessage::HEADER = "CHAT";
const std::string TimeMessage::HEADER = "TIME";
const std::string PingMessage::HEADER = "PING";

// ChatMessage -----------------------------------------------------------------

void ChatMessage::Chat(const std::string& newChat)
{
    _chat = newChat;
    if (_chat.size() > MAX_CHAT_SIZE)
        _chat.resize(MAX_CHAT_SIZE);
}

std::vector<char> ChatMessage::Serialize() const
{
    std::vector<char> rv(HEADER.begin(), HEADER.end());
    rv.push_back((char)_chat.size());
    std::copy(Chat().begin(), Chat().end(), back_inserter(rv));

    return rv;
}

size_t ChatMessage::Deserialize(const char* const msg)
{
    size_t size = msg[Message::HEADER_LENGTH];
    Chat(std::string(&msg[HEADER_LENGTH + 1], size));
    return Message::HEADER_LENGTH + 1 + size;
}

// PingMessage -----------------------------------------------------------------

std::vector<char> PingMessage::Serialize() const
{
    std::vector<char> rv(HEADER.begin(), HEADER.end());
    rv.resize(Message::HEADER_LENGTH + 2);
    memcpy(&rv.data()[4], &_ticks, 2);

    return rv;
}

size_t PingMessage::Deserialize(const char* const msg)
{
    memcpy(&_ticks, &msg[Message::HEADER_LENGTH], 2);
    return Message::HEADER_LENGTH + 2;
}

// MessageFactory --------------------------------------------------------------

std::unique_ptr<Message> MessageFactory::Get(std::string header)
{
    if (header == ChatMessage::HEADER)
        return std::unique_ptr<Message>(new ChatMessage());
    else if (header == TimeMessage::HEADER)
        return std::unique_ptr<Message>(new TimeMessage());
    else if (header == PingMessage::HEADER)
        return std::unique_ptr<Message>(new PingMessage());
    else
        return std::unique_ptr<Message>(new VoidMessage());
}

// MessageBuffer ---------------------------------------------------------------

MessageBuffer::MessageBuffer(const MessageBuffer& rhs)
{
    for (auto& msg_up : rhs._messages)
    {
        auto msgCopy_ptr = msg_up->copy();
        _messages.push_back(std::unique_ptr<Message>(msgCopy_ptr));
    }
}

std::unique_ptr<Message> MessageBuffer::Pop(void)
{
    std::unique_ptr<Message> msg;
    if (!_messages.empty())
    {
        msg = std::move(_messages.back());
        _messages.pop_back();
    }
    return msg;
}

std::vector<char> MessageBuffer::Serialize(void) const
{
    std::vector<char> rv((size_t)HEADER_LENGTH);
    uint32_t payloadSize = 0;

    rv[0] = VERSION;
    rv[1] = (char)_messages.size();

    for (auto& message : _messages)
    {
        auto s = message->Serialize();
        payloadSize += s.size();
        rv.insert(rv.end(), s.begin(), s.end());
    }

    memcpy(rv.data() + 2, &payloadSize, sizeof(uint32_t));

    return rv;
}

size_t MessageBuffer::Deserialize(char* buf)
{
    size_t totalBytes = 0;
    if (Version(buf) == VERSION)
    {
        totalBytes = HEADER_LENGTH;
        int count = Count(buf);
        Clear();
        buf += HEADER_LENGTH;

        for (int i = count; i > 0; --i)
        {
            auto msg = MessageFactory::Get(Message::GetHeader(buf));
            size_t bytes = msg->Deserialize(buf);
            Push(msg);

            totalBytes += bytes;
            buf += bytes;
        }
    }
    else
    {
        std::cerr << __FUNCTION__ << std::endl;
        std::cerr << "Versions do not match: " << Version(buf) << " " << VERSION << std::endl;
    }
    return totalBytes;
}
