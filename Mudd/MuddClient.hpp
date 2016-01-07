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

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "Message.hpp"

using boost::asio::ip::tcp;

class MuddClient
{
    public:
    MuddClient(boost::asio::io_service& ioService, tcp::resolver::iterator endpointIterator);

    void Write(MessageBuffer& msgs);

    void Close() { _ioService.post( [&]{DoClose();} ); }

    private:
    void HandleConnect(const boost::system::error_code& error);

    void HandleReadHeader(const boost::system::error_code& error);

    void HandleReadBody(const boost::system::error_code& error);

    void DoWrite(const std::vector<char>& buf);

    void HandleWrite(const boost::system::error_code& error) { if (error) DoClose(); }

    void ProcessMessageBuffer(MessageBuffer& msgs);

    void ProcessMessage(const ChatMessage& chat);

    void DoClose() { _socket.close(); }

    boost::asio::io_service& _ioService;
    tcp::socket _socket;
    std::vector<char> _readMsg;
    std::vector<char> _writeMsg;
};
