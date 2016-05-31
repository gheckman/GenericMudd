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

int ChatMessage::Deserialize(const std::vector<char>& msg, int index)
{
    BasicSerializer bs;
    index += Message::HEADER_LENGTH;
    index = bs.Deserialize(msg, _userId, index);
    index = bs.Deserialize(msg, _chat, index);
    return index;
}

#pragma endregion


#pragma region PingMessage

std::vector<char> PingMessage::Serialize() const
{
    std::vector<char> rv;
    BasicSerializer bs;
    bs.Serialize(rv, Header());
    bs.Serialize(rv, _ticks);
    return rv;
}

int PingMessage::Deserialize(const std::vector<char>& msg, int index)
{
    BasicSerializer bs;
    index += Message::HEADER_LENGTH;
    index = bs.Deserialize(msg, _ticks, index);
    return index;
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

std::unique_ptr<Message> MessageBuffer::Pop()
{
    std::unique_ptr<Message> msg;
    if (!_messages.empty())
    {
        msg = std::move(_messages.back());
        _messages.pop_back();
    }
    return msg;
}

static int PayloadSize(const std::vector<char>& buf)
{
    uint32_t payloadSize;
    BasicSerializer bs;
    bs.Deserialize(buf, payloadSize, 4);
    return payloadSize;
}

std::vector<char> MessageBuffer::Serialize() const
{
    std::vector<char> rv, header;
    BasicSerializer bs;
    uint32_t payloadSize = 0;

    for (auto& message : _messages)
    {
        auto s = message->Serialize();
        payloadSize += s.size();
        rv.insert(rv.end(), s.begin(), s.end());
    }

    bs.Serialize(header, static_cast<uint8_t>(VERSION));
    bs.Serialize(rv, static_cast<uint8_t>(_messages.size()));
    bs.Serialize(rv, payloadSize);

    return rv;
}

int MessageBuffer::Deserialize(const std::vector<char>& buf, int index)
{
    Clear();

    if (Version(buf) == VERSION)
    {
        index += HEADER_LENGTH;
        auto count = Count(buf);

        for (auto i = count; i > 0; --i)
        {
            auto msg = MessageFactory::Get(Message::GetHeader(buf, index));
            index = msg->Deserialize(buf, index);
            Push(msg);
        }
    }
    else
    {
        index = -1;
        std::cerr << __FUNCTION__ << std::endl;
        std::cerr << "Versions do not match: " << Version(buf) << " " << VERSION << std::endl;
    }

    return index;
}

#pragma endregion 
