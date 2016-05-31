#pragma once

#include "CommonConst.hpp"

#include <algorithm>
#include <cstdint>
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

    Message() { }

    virtual ~Message() { }

    virtual Message* copy() const = 0;

    virtual std::string Header() const { return HEADER; };

    static std::string GetHeader(const std::vector<char>& msg, const int index) { return std::string(msg[index], msg[index + Message::HEADER_LENGTH]); }

    virtual std::vector<char> Serialize() const { std::string header = Header(); return std::vector<char>(header.begin(), header.end()); }

    virtual int Deserialize(const std::vector<char>& msg, const int index) { return Message::HEADER_LENGTH; }
};

class VoidMessage : public Message
{
    public:
    static const std::string HEADER;

    Message* copy() const override { return new VoidMessage(*this); };

    virtual std::string Header() const override { return HEADER; };
};

class UsernameMessage : public Message
{
    public:
    static const std::string HEADER;

    Message* copy() const override { return new UsernameMessage(*this); };

    virtual std::string Header() const override { return HEADER; };

    private:
    int _userId;
    std::string _username;
};

class ChatMessage : public Message
{
    public:
    static const std::string HEADER;

    ChatMessage() = default;

    ChatMessage(const int userId, const std::string& chat) : _userId(userId), _chat(chat) {  }

    Message* copy() const override { return new ChatMessage(*this); };

    virtual std::string Header() const override { return HEADER; };

    int UserId() const { return _userId; }

    std::string Chat() const { return _chat; }

    void Chat(const std::string& chat) { _chat = chat.substr(0, CHAT_MAX); }

    void UserId(const uint8_t userId) { _userId = userId; }

    size_t Size() const { return HEADER_LENGTH + 1 + Chat().size(); }

    virtual std::vector<char> Serialize() const override;

    virtual int Deserialize(const std::vector<char>& msg, const int index) override;

    private:
    uint8_t _userId;
    std::string _chat;
};

class TimeMessage : public Message
{
    public:
    static const std::string HEADER;

    Message* copy() const override { return new TimeMessage(*this); };

    virtual std::string Header() const override { return HEADER; };
};

class PingMessage : public Message
{
    public:
    static const std::string HEADER;

    PingMessage() { }

    PingMessage(const uint16_t& ticks) { Ticks(ticks); }

    void Ticks(const uint16_t& ticks) { _ticks = ticks; }

    uint16_t Ticks() const { return _ticks; }

    Message* copy() const override { return new PingMessage(*this); };

    virtual std::string Header() const override { return HEADER; };

    virtual std::vector<char> Serialize() const override;

    virtual int Deserialize(const std::vector<char>& msg, const int index) override;

    private:
        uint16_t _ticks;
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

    std::unique_ptr<Message> Pop();

    void Clear() { _messages.clear(); }

    static int Version(const std::vector<char>& buf) { return buf[0]; }

    static int Count(const std::vector<char>& buf) { return buf[1]; }

    static int PayloadSize(const std::vector<char>& buf);

    std::vector<char> Serialize() const;

    int Deserialize(const std::vector<char>& msg, const int index);

    private:
    std::list<std::unique_ptr<Message>> _messages;
};
