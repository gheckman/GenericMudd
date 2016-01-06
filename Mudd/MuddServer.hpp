//
// ChatServer.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "Message.hpp"

using boost::asio::ip::tcp;
using std::cout;
using std::endl;

//----------------------------------------------------------------------

class MuddUser
{
    public:
    virtual ~MuddUser() {}
    virtual void Deliver(MessageBuffer msg) = 0;
};

typedef boost::shared_ptr<MuddUser> MuddUser_sp;

//----------------------------------------------------------------------

class ChatRoom
{
    public:
    void Join(MuddUser_sp user)
    {
        _users.insert(user);
    }

    void Leave(MuddUser_sp user)
    {
        _users.erase(user);
    }

    void Deliver(const ChatMessage& chat)
    {

        MessageBuffer msgs;
        msgs.Push(std::unique_ptr<Message>(new ChatMessage(chat)));

        for (auto& user : _users)
            user->Deliver(msgs);
    }

    private:
    enum { MAX_RECENT_MSGS = 100 };
    std::set<MuddUser_sp> _users;
};

//----------------------------------------------------------------------

class MuddComm : public MuddUser, public boost::enable_shared_from_this<MuddComm>
{
    public:
    MuddComm(boost::asio::io_service& ioService, ChatRoom& room) : _socket(ioService), _room(room)
    {}

    tcp::socket& Socket() { return _socket; }

    void Start()
    {
        _room.Join(shared_from_this());

        // Reserve space for the header, then read it in
        _readMsg.resize(MessageBuffer::HEADER_LENGTH, '\0');
        boost::asio::async_read(_socket,
                                boost::asio::buffer(_readMsg.data(), MessageBuffer::HEADER_LENGTH),
                                boost::bind(&MuddComm::HandleReadHeader, shared_from_this(), boost::asio::placeholders::error));
    }

    void Deliver(MessageBuffer msgs)
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

    void HandleReadHeader(const boost::system::error_code& error)
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

    void HandleReadBody(const boost::system::error_code& error)
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

    void HandleWrite(const boost::system::error_code& error)
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

    void ProcessMessageBuffer(MessageBuffer& msgs)
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

    void ProcessMessage(const ChatMessage& chat)
    {
        _room.Deliver(chat);
    }

    void ProcessMessage(const TimeMessage& timeMsg)
    {
        MessageBuffer msgs;
        auto now = time(0);
        auto s = ctime(&now);
        auto timeChat = std::unique_ptr<Message>(new ChatMessage(s));
        msgs.Push(std::move(timeChat));

        Deliver(msgs);
    }

    private:
    tcp::socket _socket;
    ChatRoom& _room;
    char _tempReadMsg[1024];
    std::vector<char> _readMsg;
    std::vector<char> _writeMsg;
    std::deque<MessageBuffer> _writeMsgs;
};

typedef boost::shared_ptr<MuddComm> MuddComm_sp;

//----------------------------------------------------------------------

class MuddServer
{
    public:
    MuddServer(boost::asio::io_service& ioService, const tcp::endpoint& endpoint) :
        _ioService(ioService),
        _acceptor(ioService, endpoint)
    {
        StartAccept();
    }

    void StartAccept()
    {
        MuddComm_sp new_session(new MuddComm(_ioService, _room));
        _acceptor.async_accept(new_session->Socket(),
                               boost::bind(&MuddServer::HandleAccept, this, new_session, boost::asio::placeholders::error));
    }

    void HandleAccept(MuddComm_sp session, const boost::system::error_code& error)
    {
        if (!error)
            session->Start();

        StartAccept();
    }

    private:
    boost::asio::io_service& _ioService;
    tcp::acceptor _acceptor;
    ChatRoom _room;
};

typedef boost::shared_ptr<MuddServer> MuddServer_ptr;
typedef std::list<MuddServer_ptr> MuddServer_list;

//----------------------------------------------------------------------