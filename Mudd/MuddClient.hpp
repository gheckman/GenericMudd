//
// MuddClient.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "Message.hpp"

#include <chrono>
#include <vector>

#include <boost/asio.hpp>

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;

class MuddClient
{
    public:
    MuddClient(io_service& ioService, tcp::resolver::iterator endpointIterator);

    void Write(MessageBuffer& msgs);

    void Close() { _ioService.post( [&]{DoClose();} ); }

    private:
    void HandleConnect(const error_code& error);

    void HandleReadHeader(const error_code& error);

    void HandleReadBody(const error_code& error);

    void DoWrite(const std::vector<char>& buf);

    void HandleWrite(const error_code& error) { if (error) DoClose(); }

    void ProcessMessageBuffer(MessageBuffer& msgs);

    void ProcessMessage(const ChatMessage& chat);
    void ProcessMessage(const PingMessage& ping);

    void DoClose() { _socket.close(); }

    io_service& _ioService;
    tcp::socket _socket;
    int _userId;
    std::string _username;
    std::vector<char> _readMsg;
    std::vector<char> _writeMsg;
};
