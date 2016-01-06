#include "MuddServer.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>

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
    boost::asio::async_read(_socket,
                            boost::asio::buffer(_readMsg.data(), MessageBuffer::HEADER_LENGTH),
                            boost::bind(&MuddComm::HandleReadHeader, shared_from_this(), boost::asio::placeholders::error));
}

void MuddComm::Deliver(MessageBuffer msgs)
{
    bool WriteInProgress = !_writeMsgs.empty();
    _writeMsgs.push_back(std::move(msgs));
    if (!WriteInProgress)
    {
        _writeMsg = _writeMsgs.front().Serialize();
        boost::asio::async_write(_socket,
                                    boost::asio::buffer(_writeMsg.data(), _writeMsg.size()),
                                    boost::bind(&MuddComm::HandleWrite, shared_from_this(), boost::asio::placeholders::error));
    }
}

void MuddComm::HandleReadHeader(const boost::system::error_code& error)
{
    if (!error)
    {
        // Get the payload size from the header
        auto payloadSize = MessageBuffer::PayloadSize(_readMsg.data());

        // reserve enough space for the header and the payload, then read in the payload
        _readMsg.resize(MessageBuffer::HEADER_LENGTH + payloadSize, '\0');
        boost::asio::async_read(_socket,
                                boost::asio::buffer(_readMsg.data() + MessageBuffer::HEADER_LENGTH, payloadSize),
                                boost::bind(&MuddComm::HandleReadBody, shared_from_this(), boost::asio::placeholders::error));
    }
    else
    {
        _room.Leave(shared_from_this());
    }
}

void MuddComm::HandleReadBody(const boost::system::error_code& error)
{
    if (!error)
    {
        MessageBuffer msgs;
        msgs.Deserialize(_readMsg.data());

        ProcessMessageBuffer(msgs);

        // Reserve space for the header, then read it in
        _readMsg.resize(MessageBuffer::HEADER_LENGTH, '\0');
        boost::asio::async_read(_socket,
                                boost::asio::buffer(_readMsg.data(), MessageBuffer::HEADER_LENGTH),
                                boost::bind(&MuddComm::HandleReadHeader, shared_from_this(), boost::asio::placeholders::error));
    }
    else
    {
        _room.Leave(shared_from_this());
    }
}

void MuddComm::HandleWrite(const boost::system::error_code& error)
{
    if (!error)
    {
        _writeMsgs.pop_front();
        if (!_writeMsgs.empty())
        {
            _writeMsg = _writeMsgs.front().Serialize();
            boost::asio::async_write(_socket,
                                        boost::asio::buffer(_writeMsg.data(), _writeMsg.size()),
                                        boost::bind(&MuddComm::HandleWrite, shared_from_this(), boost::asio::placeholders::error));
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

// MuddServer ------------------------------------------------------------------

void MuddServer::StartAccept()
{
    MuddComm_sp new_session(new MuddComm(_ioService, _room));
    _acceptor.async_accept(new_session->Socket(),
                            boost::bind(&MuddServer::HandleAccept, this, new_session, boost::asio::placeholders::error));
}

void MuddServer::HandleAccept(MuddComm_sp session, const boost::system::error_code& error)
{
    if (!error)
        session->Start();

    StartAccept();
}