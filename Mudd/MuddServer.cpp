#include "MuddServer.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <functional>

using boost::asio::async_read;
using boost::asio::buffer;
using std::bind;
using std::placeholders::_1;

// ChatRoom --------------------------------------------------------------------

void ChatRoom::Deliver(const ChatMessage& chat)
{
    MessageBuffer msgs;
    msgs.Push(std::unique_ptr<Message>(new ChatMessage(chat)));

    for (auto& user : _users)
        user->Deliver(msgs);
}

// MuddComm --------------------------------------------------------------------

void MuddComm::Start()
{
    _room.Join(shared_from_this());

    // Reserve space for the header, then read it in
    _readMsg.resize(MessageBuffer::HEADER_LENGTH, '\0');

    async_read(_socket, buffer(_readMsg.data(), MessageBuffer::HEADER_LENGTH), bind(&MuddComm::HandleReadHeader, shared_from_this(), _1));
}

void MuddComm::Deliver(MessageBuffer msgs)
{
    bool WriteInProgress = !_writeMsgs.empty();
    _writeMsgs.push_back(std::move(msgs));
    if (!WriteInProgress)
    {
        _writeMsg = _writeMsgs.front().Serialize();
        async_write(_socket, buffer(_writeMsg.data(), _writeMsg.size()), bind(&MuddComm::HandleWrite, shared_from_this(), _1));
    }
}

void MuddComm::HandleReadHeader(const error_code& error)
{
    if (!error)
    {
        // Get the payload size from the header
        auto payloadSize = MessageBuffer::PayloadSize(_readMsg.data());

        // reserve enough space for the header and the payload, then read in the payload
        _readMsg.resize(MessageBuffer::HEADER_LENGTH + payloadSize, '\0');
        async_read(_socket, buffer(_readMsg.data() + MessageBuffer::HEADER_LENGTH, payloadSize), bind(&MuddComm::HandleReadBody, shared_from_this(), _1));
    }
    else
    {
        _room.Leave(shared_from_this());
    }
}

void MuddComm::HandleReadBody(const error_code& error)
{
    if (!error)
    {
        MessageBuffer msgs;
        msgs.Deserialize(_readMsg.data());

        ProcessMessageBuffer(msgs);

        // Reserve space for the header, then read it in
        _readMsg.resize(MessageBuffer::HEADER_LENGTH, '\0');
        async_read(_socket, buffer(_readMsg.data(), MessageBuffer::HEADER_LENGTH), bind(&MuddComm::HandleReadHeader, shared_from_this(), _1));
    }
    else
    {
        _room.Leave(shared_from_this());
    }
}

void MuddComm::HandleWrite(const error_code& error)
{
    if (!error)
    {
        _writeMsgs.pop_front();
        if (!_writeMsgs.empty())
        {
            _writeMsg = _writeMsgs.front().Serialize();
            async_write(_socket, buffer(_writeMsg.data(), _writeMsg.size()), bind(&MuddComm::HandleWrite, shared_from_this(), _1));
        }
    }
    else
    {
        _room.Leave(shared_from_this());
    }
}

void MuddComm::ProcessMessageBuffer(MessageBuffer& msgs)
{
    auto msg_up = msgs.Pop();
    while (msg_up)
    {
        if (msg_up->Header() == ChatMessage::HEADER)
        {
            ChatMessage* msg = dynamic_cast<ChatMessage*>(msg_up.get());
            ProcessMessage(*msg);
        }
        else if (msg_up->Header() == TimeMessage::HEADER)
        {
            TimeMessage* msg = dynamic_cast<TimeMessage*>(msg_up.get());
            ProcessMessage(*msg);
        }
        else if (msg_up->Header() == PingMessage::HEADER)
        {
            PingMessage* msg = dynamic_cast<PingMessage*>(msg_up.get());
            ProcessMessage(*msg);
        }

        msg_up = msgs.Pop();
    }
}

void MuddComm::ProcessMessage(const TimeMessage& timeMsg)
{
    MessageBuffer msgs;
    auto now = time(0);
    auto s = ctime(&now);
    auto timeChat = std::unique_ptr<Message>(new ChatMessage(s));
    msgs.Push(std::move(timeChat));

    Deliver(msgs);
}

void MuddComm::ProcessMessage(const PingMessage& pingMsg)
{
    MessageBuffer msgs;
    auto ping_up = std::unique_ptr<Message>(new PingMessage(pingMsg.Ticks()));
    msgs.Push(ping_up);

    Deliver(msgs);
}

// MuddServer ------------------------------------------------------------------

void MuddServer::StartAccept()
{
    MuddComm_sp new_session(new MuddComm(_ioService, _room));
    _acceptor.async_accept(new_session->Socket(), bind(&MuddServer::HandleAccept, this, new_session, _1));
}

void MuddServer::HandleAccept(MuddComm_sp session, const boost::system::error_code& error)
{
    if (!error)
        session->Start();

    StartAccept();
}