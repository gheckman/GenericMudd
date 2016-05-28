#include "Message.hpp"

#include "BasicSerializer.hpp"

#include <cstring>

#pragma region Headers

const std::string Message::HEADER = "AMSG";
const std::string VoidMessage::HEADER = "VOID";
const std::string UsernameMessage::HEADER = "UNAM";
const std::string ChatMessage::HEADER = "CHAT";
const std::string TimeMessage::HEADER = "TIME";
const std::string PingMessage::HEADER = "PING";

#pragma endregion


#pragma region UsernameMessage

#pragma endregion


#pragma region ChatMessage

std::vector<char> ChatMessage::Serialize() const
{
    std::vector<char> rv;
    BasicSerializer bs;
    bs.Serialize(rv, HEADER);
    bs.Serialize(rv, _userId);
    bs.Serialize(rv, _chat);

    return rv;
}

size_t ChatMessage::Deserialize(const std::vector<char>& msg)
{
    BasicSerializer bs;
    int size = msg[Message::HEADER_LENGTH + 1];
    std::vector<char> vecMsg(msg, msg + size);

    size_t index = Message::HEADER_LENGTH;
    index = bs.Deserialize(vecMsg, _userId, index);
    index = bs.Deserialize(vecMsg, _chat, index);
    return index;
}

#pragma endregion


#pragma region PingMessage

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

#pragma endregion 


#pragma region MessageFactory

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

#pragma endregion 


#pragma region MessageBuffer

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

int MessageBuffer::PayloadSize(const std::vector<char>& buf)
{
    int payload;
    BasicSerializer bs;
    bs.Deserialize(buf, payload, 2);
}

size_t MessageBuffer::Deserialize(std::vector<char> buf)
{
    size_t totalBytes = 0;
    if (Version(buf) == VERSION)
    {
        totalBytes = HEADER_LENGTH;
        int count = Count(buf);
        Clear();

        for (int i = count; i > 0; --i)
        {
            auto msg = MessageFactory::Get(Message::GetHeader(buf.data() + totalBytes));
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

#pragma endregion 
