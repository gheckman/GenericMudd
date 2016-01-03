#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

class Message
{
    public:
    enum { HEADER_LENGTH = 4 };

    Message(void) { }

    virtual ~Message(void) { }

    virtual Message* copy() const = 0;

    const std::string& Header(void) const { return _header; }

    bool Header(std::string newHeader)
    {
        if (newHeader.size() >= HEADER_LENGTH)
        {
            _header = newHeader.substr(0, HEADER_LENGTH);
            return true;
        }
        return false;
    }

    static const std::string& HEADER(void)
    {
        static const std::string str = "AMSG";
        return str;
    }

    static std::string GetHeader(const char* const msg)
    {
        return std::string(msg, HEADER_LENGTH);
    }

    virtual std::vector<char> Serialize(void) const = 0;
    virtual size_t Deserialize(char*) = 0;

    private:
    std::string _header;
};

class VoidMessage : public Message
{
    public:
    static const std::string& HEADER(void)
    {
        static const std::string str = "VOID";
        return str;
    }

    Message* copy() const override { return new VoidMessage(*this); };

    std::vector<char> Serialize(void) const override { return std::vector<char>(); }
    size_t Deserialize(char*) override { return Message::HEADER_LENGTH; }
};

class ChatMessage : public Message
{
    public:
    enum { MAX_CHAT_SIZE = 96 };

    ChatMessage(void) : Message()
    {
        Header(HEADER());
    }

    ChatMessage(const std::string& chat) : ChatMessage()
    {
        Chat(chat);
    }

    Message* copy() const override { return new ChatMessage(*this); };

    static const std::string& HEADER(void)
    {
        static const std::string str = "CHAT";
        return str;
    }

    const std::string& Chat(void) const { return _chat; }

    void Chat(const std::string& newChat)
    {
        _chat = newChat;
        if (_chat.size() > MAX_CHAT_SIZE)
            _chat.resize(MAX_CHAT_SIZE);
    }

    size_t Size(void) const
    {
        return HEADER_LENGTH + 1 + Chat().size();
    }

    virtual std::vector<char> Serialize() const override
    {
        std::vector<char> rv(Header().begin(), Header().end());
        rv.push_back((char)_chat.size());
        std::copy(Chat().begin(), Chat().end(), std::back_inserter(rv));

        return rv;
    }

    // pre: pointer is immediatley after header
    virtual size_t Deserialize(char* msg) override
    {
        size_t size = msg[Message::HEADER_LENGTH];
        msg += Message::HEADER_LENGTH + 1;
        Chat(std::string(msg, size));
        return size + Message::HEADER_LENGTH;
    }

    private:
    std::string _chat;
};

class TimeMessage : public Message
{
    public:
    TimeMessage(void) : Message()
    {
        Header(HEADER());
    }

    Message* copy() const override { return new TimeMessage(*this); };

    static const std::string& HEADER(void)
    {
        static const std::string str = "TIME";
        return str;
    }

    virtual std::vector<char> Serialize(void) const override
    {
        return std::vector<char>(Header().begin(), Header().end());
    }

    virtual size_t Deserialize(char*) override { return Message::HEADER_LENGTH; }

};

class MessageFactory
{
    public:
    static std::unique_ptr<Message> Get(std::string header)
    {
        if (header == ChatMessage::HEADER())
            return std::unique_ptr<Message>(new ChatMessage());
        else if (header == TimeMessage::HEADER())
            return std::unique_ptr<Message>(new TimeMessage());
        else
            return std::unique_ptr<Message>(new VoidMessage());
    }
};

class MessageBuffer
{
    public:
    enum { VERSION = 1 };
    enum { HEADER_LENGTH = 6 };

    MessageBuffer() : _messages() {}

    MessageBuffer(const MessageBuffer& rhs)
    {
        for (auto& msg_up : rhs._messages)
        {
            auto msgCopy_ptr = msg_up->copy();
            _messages.push_back(std::unique_ptr<Message>(msgCopy_ptr));
        }
    }

    MessageBuffer(MessageBuffer&&) = default;

    void Push(std::unique_ptr<Message>& msg) { _messages.push_back(std::move(msg)); }

    std::unique_ptr<Message> Pop(void)
    {
        std::unique_ptr<Message> msg;
        if (!_messages.empty())
        {
            msg = std::move(_messages.back());
            _messages.pop_back();
        }
        return msg;
    }

    void Clear(void) { _messages.clear(); }

    static int Version(const char* const msg) { return msg[0]; }
    static int Count(const char* const msg) { return msg[1]; }
    static int PayloadSize(const char* const msg) { return *(int*)(&msg[2]); }

    std::vector<char> Serialize(void) const
    {
        std::vector<char> rv( (size_t)HEADER_LENGTH );
        int payloadSize = 0;

        rv[0] = VERSION;
        rv[1] = (char)_messages.size();

        auto serializeAll = [&rv, &payloadSize](const std::unique_ptr<Message>& message)
        {
            auto s = message->Serialize();
            payloadSize += s.size();
            rv.insert(rv.end(), s.begin(), s.end());
        };

        std::for_each(_messages.begin(), _messages.end(), serializeAll);
        memcpy(rv.data() + 2, &payloadSize, sizeof(int));

        return rv;
    }

    size_t Deserialize(char* buf)
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

    private:
    std::list<std::unique_ptr<Message>> _messages;
};