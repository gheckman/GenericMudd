#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <vector>

class Message
{
    public:
    enum { HEADER_LENGTH = 4 };

    static const std::string HEADER;

    Message(void) { }

    virtual ~Message(void) { }

    virtual Message* copy(void) const = 0;

    virtual std::string Header(void) const { return HEADER; };

    static std::string GetHeader(const char* const msg) { return std::string(msg, HEADER_LENGTH); }

    virtual std::vector<char> Serialize(void) const { std::string header = Header(); return std::vector<char>(header.begin(), header.end()); }

    virtual size_t Deserialize(char*) { return Message::HEADER_LENGTH; }
};

class VoidMessage : public Message
{
    public:
    static const std::string HEADER;

    Message* copy() const override { return new VoidMessage(*this); };

    virtual std::string Header(void) const override { return HEADER; };
};

class ChatMessage : public Message
{
    public:
    enum { MAX_CHAT_SIZE = 88 };

    static const std::string HEADER;

    ChatMessage(void) = default;

    ChatMessage(const std::string& chat) : ChatMessage() { Chat(chat); }

    Message* copy() const override { return new ChatMessage(*this); };

    virtual std::string Header(void) const override { return HEADER; };

    const std::string& Chat(void) const { return _chat; }

    void Chat(const std::string& newChat);

    size_t Size(void) const { return HEADER_LENGTH + 1 + Chat().size(); }

    virtual std::vector<char> Serialize() const override;

    virtual size_t Deserialize(char* msg) override;

    private:
    std::string _chat;
};

class TimeMessage : public Message
{
    public:
    static const std::string HEADER;

    Message* copy() const override { return new TimeMessage(*this); };

    virtual std::string Header(void) const override { return HEADER; };
};

class MessageFactory
{
    public:
    static std::unique_ptr<Message> Get(std::string header);
};

class MessageBuffer
{
    public:
    enum { VERSION = 1 };

    enum { HEADER_LENGTH = 6 };

    MessageBuffer() = default;

    MessageBuffer(const MessageBuffer& rhs);

    MessageBuffer(MessageBuffer&&) = default;

    void Push(std::unique_ptr<Message>& msg) { _messages.push_back(std::move(msg)); }

    std::unique_ptr<Message> Pop(void);

    void Clear(void) { _messages.clear(); }

    static int Version(const char* const msg) { return msg[0]; }

    static int Count(const char* const msg) { return msg[1]; }

    static int PayloadSize(const char* const msg) { return *(int*)(&msg[2]); }

    std::vector<char> Serialize(void) const;

    size_t Deserialize(char* buf);

    private:
    std::list<std::unique_ptr<Message>> _messages;
};
