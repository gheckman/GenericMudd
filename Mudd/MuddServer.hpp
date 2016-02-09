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

#include "Message.hpp"

#include <deque>
#include <list>
#include <memory>
#include <set>

#include <boost/asio.hpp>

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;
using std::cout;
using std::endl;

//----------------------------------------------------------------------

class MuddUser
{
    public:
    virtual ~MuddUser() {}
    virtual void Deliver(MessageBuffer msg) = 0;
};

typedef std::shared_ptr<MuddUser> MuddUser_sp;

//----------------------------------------------------------------------

class ChatRoom
{
    public:
    void Join(MuddUser_sp user) { _users.insert(user); }

    void Leave(MuddUser_sp user) { _users.erase(user); }

    void Deliver(const ChatMessage& chat);

    private:
    enum { MAX_RECENT_MSGS = 100 };
    std::set<MuddUser_sp> _users;
};

//----------------------------------------------------------------------

class MuddComm : public MuddUser, public std::enable_shared_from_this<MuddComm>
{
    public:
    MuddComm(io_service& ioService, ChatRoom& room) : _socket(ioService), _room(room)
    {}

    tcp::socket& Socket() { return _socket; }

    void Start();

    void Deliver(MessageBuffer msgs);

    void HandleReadHeader(const error_code& error);

    void HandleReadBody(const error_code& error);

    void HandleWrite(const error_code& error);

    void ProcessMessageBuffer(MessageBuffer& msgs);

    void ProcessMessage(const ChatMessage& chatMsg) { _room.Deliver(chatMsg); }

    void ProcessMessage(const TimeMessage& timeMsg);

    void ProcessMessage(const PingMessage& pingMsg);

    private:
    tcp::socket _socket;
    ChatRoom& _room;
    std::vector<char> _readMsg;
    std::vector<char> _writeMsg;
    std::deque<MessageBuffer> _writeMsgs;
};

typedef std::shared_ptr<MuddComm> MuddComm_sp;

//----------------------------------------------------------------------

class MuddServer
{
    public:
    MuddServer(io_service& ioService, const tcp::endpoint& endpoint) :
        _ioService(ioService),
        _acceptor(ioService, endpoint)
    {
        StartAccept();
    }

    void StartAccept();

    void HandleAccept(MuddComm_sp session, const error_code& error);

    private:
    io_service& _ioService;
    tcp::acceptor _acceptor;
    ChatRoom _room;
};

typedef std::shared_ptr<MuddServer> MuddServer_ptr;
typedef std::list<MuddServer_ptr> MuddServer_list;

//----------------------------------------------------------------------